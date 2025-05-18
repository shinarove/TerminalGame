project('TerminalGame', 'c',
        version : '1.0.0',
        default_options : ['warning_level=3', 'c_std=gnu2x'])

termbox_file = files('termbox2/termbox2.c')

helper_files = files('src/helper/string_helper.c',)

data_types_files = files('src/data_types/cache/string_cache.c')

src_files = files('src/main.c',
                  'src/game.c',)

memory_files = files('src/memory/mem_mgmt.c')

thread_files = files('src/thread/thread_handler.c')

logger_files = files('src/logger/logger.c',
                     'src/logger/ringbuffer.c')

io_files = files('src/io/menu.c')

input_files = files('src/io/input/input_handler.c')

output_files = files('src/io/output/output.c',
                     'src/io/output/common/common_output.c',
                     'src/io/output/specific/map_output.c',
                     'src/io/output/specific/character_output.c',)

local_files = files('src/io/local/local_handler.c',)

menu_files = files('src/game_modes/menus/title_screen_mode.c',
                   'src/game_modes/menus/main_menu_mode.c',
                   'src/game_modes/menus/language_menu_mode.c',
                   'src/game_modes/menus/save_game_mode.c',
                   'src/game_modes/menus/load_game_mode.c',)

map_files = files('src/game_data/map/map.c',
                  'src/game_modes/map/map_mode.c',
                  'src/game_modes/map/map_event_handler.c',
                  'src/game_data/map/map_generator.c',
                  'src/game_data/map/map_populator.c',
                  'src/game_data/map/map_parser.c',
                  'src/game_data/map/map_revealer.c',)

combat_files = files('src/game_modes/combat/combat_mode.c')

ability_files = files('src/game_data/ability/ability.c')

item_files = files('src/game_data/item/gear.c')

inventory_files = files('src/game_data/inventory/inventory.c',
                        'src/game_modes/inventory/inventory_mode.c')

character_files = files('src/game_modes/character/lvl_up_mode.c',
                        'src/game_modes/character/character_creation_mode.c',
                        'src/game_modes/character/character_mode.c',
                        'src/game_data/character/character.c',
                        'src/game_data/character/character_parser.c',
                        'src/game_data/character/character_save_handler.c',
                        'src/game_data/character/enemy_generator.c',)

game_mechanics_files = files('src/game_mechanics/ability_usage.c',
                             'src/game_mechanics/leveling.c',
                             'src/game_mechanics/luck.c',
                             'src/game_mechanics/dice/dice.c')

game_data_files = files('src/game_data/save_file_handler.c')

TerminalGame = executable('TerminalGame',
                          termbox_file,
                          data_types_files,
                          helper_files,
                          src_files,
                          memory_files,
                          thread_files,
                          logger_files,
                          io_files,
                          input_files,
                          output_files,
                          local_files,
                          menu_files,
                          map_files,
                          combat_files,
                          ability_files,
                          item_files,
                          inventory_files,
                          character_files,
                          game_mechanics_files,
                          game_data_files,
                          install : true)

subdir('test') # Test directory
