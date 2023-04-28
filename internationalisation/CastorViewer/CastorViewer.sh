# example sh script to generate catalog file
# it searches for strings marked with _() in all .cpp files in directory ./src
# the generated file is called 'app_name.pot' and is generated in ./po
CPP_FILE_LIST=`find ../../tools/GuiCommon ../../tools/CastorViewer -name '*.cpp'`
xgettext -d CastorViewer -C -s --keyword=_ -p ./po/ -o CastorViewer.pot $CPP_FILE_LIST
