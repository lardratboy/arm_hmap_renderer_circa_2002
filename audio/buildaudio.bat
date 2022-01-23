@echo ENCODE ----------------------------------------
call encode.bat %1 %2 %3 %4 %5 %6 %7 %8 %9
@echo DECODE ----------------------------------------
call decode.bat %1 %2 %3 %4 %5 %6 %7 %8 %9
@echo REFORMAT --------------------------------------
bin2code %1.raw %1_raw.cpp -b8 -o32 -naudio_l_channel
bin2code %1_(%2)Adpcm.raw %1_(%2)Adpcm.cpp -b8 -o32 -naudio_l_channel
@echo DONE ------------------------------------------

