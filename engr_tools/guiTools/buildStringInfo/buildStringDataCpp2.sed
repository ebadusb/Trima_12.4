1i
1i
1iStringTable stringTable[] =
1i{
s/STRING_FILE_TABLE_START//
s/STRING_FILE_TABLE_END//
#   token name                   "text"      red       green     blue      style       x         y         w         h         fontsize   localization * 
s/^\([A-Za-z_][A-Za-z0-9_]*\)[ \t]*["].*["][ \t]*[0-9]*[ \t]*[0-9]*[ \t]*[0-9]*[ \t]*0x[0-9]*[ \t]*[0-9]*[ \t]*[0-9]*[ \t]*[0-9]*[ \t]*[0-9]*[ \t]*[0-9]*[ \t]*\([0-9]*\).*/\ { StringData::\1, "\1", \2 },/p
$i};
$i
$i
$iStringData::StringData() : TrimaStringData()
$i{
$i\   for (int i = 0; i < (sizeof(stringTable)/sizeof(stringTable[0])); ++i)
$i\   {
$i\      stringTable[i].item.setId(stringTable[i].id);
$i\      stringTable[i].item.englishOnly(stringTable[i].localize == 0);
$i\      _textMap[stringTable[i].id] = &stringTable[i].item;
$i\   }
$i}
$i
$i
$iStringData::~StringData()
$i{
$i}