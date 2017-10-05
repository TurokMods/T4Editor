built application expects to see a 'config.txt' file in the working directory

config parser supports lines of the following formats:

var_name var_value

var_name 'var value with spaces'


--REQUIRED CONFIG VARIABLES--

game_data_path, path, (eg. 'E:\Games\Turok\data') - must be in single quotes if the path includes spaces

editor_data_path, path, (similar to game_data_path, path to the assets the editor needs)


--NOT REQUIRED CONFIG VARIABLES--

res_x, integer, initial editor window width

res_y, integer, initial editor window height

