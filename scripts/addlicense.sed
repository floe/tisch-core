# sed script to insert "header.txt" above the first line
/\*\*\*\*\*\*\*\*\*\*\\/{ h; b done; }
1{ h; r license.txt
D; }
2{ x; G; }
:done
1{ d; }
#---end of sed script---
