# example sh script to generate catalog file
# it searches for strings marked with _() in all .cpp files in directory ./src
# the generated file is called 'app_name.pot' and is generated in ./po
CPP_FILE_LIST=`find ../source/ImgConverter -name '*.cpp' -print`
xgettext -d CastorViewer -s --keyword=_ -p ../po -o ImgConverter.pot $CPP_FILE_LIST
