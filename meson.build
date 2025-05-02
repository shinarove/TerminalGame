project('TerminalGame', 'c',
        version : '1.0.0',
        default_options : ['warning_level=3', 'c_std=gnu2x'])

termbox_file = files('termbox2/termbox2.c')

src_files = files('src/main.c')

memory_files = files('src/memory/mem_mgmt.c')

thread_files = files('src/thread/thread_handler.c')

logger_files = files('src/logger/logger.c',
                     'src/logger/ringbuffer.c')

input_files = files('src/io/input/input_handler.c')

map_files = files('src/game_data/map/map_generator.c',
                  'src/game_data/map/map_populator.c',)

TerminalGame = executable('TerminalGame',
                          termbox_file,
                          src_files,
                          memory_files,
                          thread_files,
                          logger_files,
                          input_files,
                          map_files,
                          install : true)

