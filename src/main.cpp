#include "audio/audio.hpp"
#include "audio/fft.hpp"
#include "entries.hpp"
#include "renderer/renderer.hpp"
#include "util.hpp"
#include "window/window.hpp"
#include <SDL3/SDL.h>
#include <cmath>

#include <iostream>

const SDL_Color background = {76, 86, 106, 255};
const SDL_Color smear_col = {163, 190, 140, 255};
const SDL_Color box_col = {94, 129, 172, 255};

static bool init_sdl(void);
static void quit_sdl(void);

int main(int argc, char **argv)
{
    if (!init_sdl()) {
        log_write_str("Failed to initialize SDL3:", SDL_GetError());
        return 1;
    }

    contents entries = contents(strvec(0), strvec(0), false, true);
    if (argc > 1 && argc < 3) {
        std::string dir = std::string(argv[1]);
        entries = get_directory_contents(dir);
    } else {
        std::cout << "Usage: sv relative/path/to/directory" << std::endl;
        return 0;
    }

    if (!entries.valid || entries.empty) {
        log_write_str("Entries marked invalid or empty", "");
        return 1;
    }

    audio_streambuffer stream;
    if (!stream.set_device_id(stream.open_device())) {
        log_write_str("Failed to open default audio device:", SDL_GetError());
        return 1;
    }
    stream.pause_audio();

    strvec::iterator entry_iterator = entries.entry_paths.begin();
    std::unique_ptr<audio_data> data = std::make_unique<audio_data>(nullptr, vecf64(), meta_data(0, 0, 0, 0), false);
    const char *path = (*entry_iterator).c_str();
    *data = read_file(open_file(path));

    if (!stream.set_stream_ptr(stream.create_stream(stream.spec_from_file(data)))) {
        log_write_str("Failed create audio stream:", SDL_GetError());
        return 1;
    }

    if (!stream.set_audio_callback(data)) {
        log_write_str("Failed to assign audio get callback:", SDL_GetError());
        return 1;
    }

    if (!stream.audio_stream_bind()) {
        log_write_str("Failed to bind stream to device:", SDL_GetError());
        return 1;
    }
    stream.resume_audio();

    window win = window("sv", 400, 300, SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE);
    win.set_window(win.create());
    renderer rend = renderer(nullptr, win.get_width(), win.get_height(), background, smear_col, box_col);
    rend.set_renderer(rend.create(win.get_window()));

    std::vector<grid_pos> vertices = {
        grid_pos(0.5f, 0.5f, 0.5f),
        grid_pos(-0.5f, 0.5f, 0.5f),
        grid_pos(-0.5f, -0.5f, 0.5f),
        grid_pos(0.5f, -0.5f, 0.5f),

        grid_pos(0.5f, 0.5f, -0.5f),
        grid_pos(-0.5f, 0.5f, -0.5f),
        grid_pos(-0.5f, -0.5f, -0.5f),
        grid_pos(0.5f, -0.5f, -0.5f),
    };

    std::vector<indice4> indices = {
        indice4(0, 1, 2, 3),
        indice4(4, 5, 6, 7),
        indice4(1, 5, 6, 2),
        indice4(0, 3, 7, 4),
        indice4(0, 4, 5, 1),
        indice4(3, 2, 6, 7),
    };

    std::vector<edge> edges = rend.make_edges(indices);
    std::vector<indice3> triangle_indices = rend.quad_to_triangle(indices);
    transformer fft;

    SDL_ShowWindow(win.get_window());
    SDL_EnableScreenSaver();

    // a = 1 - e(-t / time_constant)
    const u32 FPS = 120;
    const f32 frame_alpha = 1.0 - exp(-1.0 / (FPS * 0.08));

    const u32 frame_gate = 1000 / FPS;
    bool running = true;

    // f32 dz = 0.0f;
    f32 angle = 0.0f;

    std::vector<freq_range> ranges = gen_bins();
    std::vector<f64> bin_sums(FREQUENCY_BINS);
    std::vector<f64> smooth_bin_sums(FREQUENCY_BINS);
    std::vector<f64> smear_bin_sums(FREQUENCY_BINS);

    print_bins(ranges);

    while (running) {
        u64 start = SDL_GetTicks();
        rend.colour(background.r, background.g, background.b, background.a);
        rend.clear();

        if (data->valid && data->meta.position >= data->meta.samples) {
            stream.pause_audio();
            entry_iterator = get_next_entry(strvec_view(entries.entry_paths, entry_iterator));
            *data = read_file(open_file(entry_iterator->c_str()));
            stream.resume_audio();
        }

        if (data->valid && data->meta.position < data->meta.samples) {
           bin_sums = fft.fft_exec(data->fft_in, data->meta.sample_rate, ranges);
           for(u32 i = 0; i < bin_sums.size(); i++){
            smooth_bin_sums[i] += interpolate(bin_sums[i], smooth_bin_sums[i], frame_alpha);
            smear_bin_sums[i] += interpolate(smooth_bin_sums[i], smear_bin_sums[i], frame_alpha);
           }
        }

        // dz += 1.0f * (1.0f / 60);
        angle += (PI * (1.0f / FPS) * 0.5);

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            default:
                break;

            case SDL_EVENT_WINDOW_RESIZED:
            {
                win.win_size_update();
                rend.update_draw_plane(win.get_width(), win.get_height());
            } break;

            case SDL_EVENT_QUIT:
                running = false;
                break;
            }
        }

        rend.draw_cube_lines(smear_bin_sums, vertices, triangle_indices, rend.get_smear_col());
        rend.draw_cube_lines(smooth_bin_sums, vertices, triangle_indices, rend.get_box_col());

        // rend.draw_points(&translated);
        rend.present();

        u64 frame_time = SDL_GetTicks() - start;
        if (frame_time < frame_gate) {
            u32 delay = static_cast<u32>(frame_gate - frame_time);
            SDL_Delay(delay);
        }
    }

    stream.audio_device_close();
    stream.stream_destroy();
    quit_sdl();
    return 0;
}

bool init_sdl(void) { return SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS); }

void quit_sdl(void) { SDL_Quit(); }
