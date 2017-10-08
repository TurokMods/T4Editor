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



--DISCOVERED ACTOR VARIABLE TYPES--
(add these to your config.txt under [ACTOR VARIABLES])

av__FDMULT float
av__IGNALBOX string
av__CONEVISION string
av__A_NDD float
av__HIDE float
av__STCHS float
av__TURNINGSPEED float
av__SLOWTURN string
av__FLEER float
av__LEASHED string
av__LEASHEDTOREGION string
av__AD_TD float
av__STSSPD float
av__TIMETOTURNOFF float
av__CHEVADE string
av__STAMN float
av__STSAMN float
av__A_MDD float
av__A_SOFF float
av__HEALTH float
av__SOUNDRADIUS float
av__CUCCOVER string
av__MEDIUMRANGEDIST float
av__SXZANGLE float
av__LONGRANGEDIST float
av__AU_A float
av__CRITICALTHRESHHOLD float
av__INITIALSTATE string
av__LEASHRADIUS float
av__CLOSERANGEDIST float
av__TYPE float
av__AU_D float
av__USEHEADTRACKING string
av__TIMETOREACHMAXSTRENGTH float
av__HDSOUND string
av__IGNOREPLAYER string
av__SIGHTRADIUS float
av__LDSOUND string
av__MDSOUND string
av__MAXHEALTH float
av__ALWAYSRUN string
av__SYANGLE float
