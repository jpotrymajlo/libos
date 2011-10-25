cd ..
rem rm -r libosdoc
cd scripts
doxygen doxyfile

for %%f in (..\\libosdoc\\html\\*.html) do call slim w i %%f %%f_temp


