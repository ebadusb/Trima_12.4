########### Update Foreign Language Text Table for Focussed GUI ##############
#
# This program has two phases: 1) convert the ILE translations of the Focussed
# GUI text into valid C++ text string literals compatible with the LANGUAGE
# array. 2) Translate the English LANGUAGE array found in the stringen.cpp file
# into a version for the target language's table.
#
# Input: ex. gawk -f updtocpp.awk -v LANG=F -v LNAME=french frconv.txt ..\current_stringtb\stringtb.cpp > stringfr.cpp
#
#   -f filename = in this example, iletocpp.awk. This program. it should be qualified with path info.
#   -v LANG=?   = this sets the langauge qualifying letter that prefaces the language symbols
#                    see below for conventions.
#   -v LNAME=?  = this sets the name of the language, used for setting table name
#   file1       = in example, "frconv.txt". This is the file containing the Excel exported
#                    ILE language translation. It will be the input to creating text literals.
#   file2       = in example, "..\current_stringtb\stringtb.cpp". This is the English LANGUAGE table
#                    file. This is considered to be the "gold standard" for completeness and
#                    will be used as the source to create a version of the table for the target language.
#
# Output: ref. example above.
#
#   file3       = in example, "stringfr.cpp". This is the re-directed output of the program
#                    which is the C++ source for the target language's LANGUAGE table and literals.
#




# This array will hold a list of all the defined text keys.

function store_elements(key, language_literal, lang_char) {
   language_literal_array [key] = 1 ;
}


# This array will hold a list of all the text literals not referenced
#   by the styling array.

function litnotfound_elements(key) {
   language_nolit_array [key] = 1 ;
}

# This array will hold a list of all the text literals referenced by
#   the styling array.

function litreferenced_elements(key) {
   language_litrefed_array [key] = 1 ;
}


# This takes a styling record symbol and strips off any trailing comma's
#   or comment garbage.

function strip_symbol(raw_symbol) {
   slen = length(raw_symbol) ;
   for(i = slen; i > 0; i--) {
      # stop stripping as soon as the first uppercase character is found
      #  searching from back to front.
      if ((substr(raw_symbol, i, 1) >= "A" && substr(raw_symbol, i, 1) <= "Z") || (substr(raw_symbol, i, 1) >= "0" && substr(raw_symbol, i, 1) <= "9")) {
         return(substr(raw_symbol,1, i));
      } # end if
   } # end for
} # end function





BEGIN {
   #create the file checksum accumulators
   acc = 0;
   accstr = 0;

   #create an indicator of when we're scanning the LANGUAGE table for English
   instyle_flag = 0;

   #define a checksum mod number
   CSDIVISOR = 751395311 ;

   #The language prefix character:
   # "LANG" is assigned on the command line invoking this AWK program
   # and may be one of the characters shown below.

   #language_prefix = "G" ;  # German
   #language_prefix = "I" ;  # Italian
   #language_prefix = "S" ;  # Spanish
   #language_prefix = "J" ;  # Japanese
   #language_prefix = "F" ;  # French
   #language_prefix = "D" ;  # Dutch
   #language_prefix = "P" ;  # Portuguese

   new_prefix = LANG "DCL_";

   #Set the language name for customizing the table name.

   language_name = LNAME;

   # Create the stringtb.cpp file name literal for comparisons below
###   TABLE_FILENAME = "..\\curstrtb\\stringtb.cpp" ;
   TABLE_FILENAME = "translat.h" ;


   # Create variable for defining which section we are in and initialize it
   # SECTIONID=0 - In no section
   # SECTIONID=1 - In new string section
   # SECTIONID=2 - In update string section
   # SECTIONID=3 - In new styling record section
   # SECTIONID=4 - In update styling record section
   # SECTIONID=5 - In new manifest section

   SECTIONID=0;

} #### End of BEGIN block


# Print out the file names inline in the code.

 { if(FILENAME != oldfile) {
    oldfile = FILENAME;
# Set SECTIONID to zero at start of each new file
    SECTIONID=0;
# If this is the table file, add an end section for the last file
    if (FILENAME==TABLE_FILENAME) {
       printf("\n");
       printf("<<>>\n");
       printf("<<SECTION END>>\n");
       printf("<<>>\n");
    }
# Print out header information neccesary to compile
    if (FILENAME!=TABLE_FILENAME) {
       printf("<<>> String updates for %s on Trima\n",language_name);
       printf("<<>>\n");
       printf("<<>>\n");
       printf("<<>>\n");
       printf("<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
       printf("\n");
    }
   }
}



# Processing: The following block will take the ILE file and create the C++ text
# string literals for the target language. These literals are referred to by the
# LANGUAGE table copied and translated at the end of this AWK program.
# This block assumes that new string definitions begin with DCL_TEXT_,
# and updated string definitions begin with UDCL_TEXT. It also assumes that all
# new strings come first and all updated strings come after that.
# In this new version, the translated text is assumed to start with an equals sign
#   (=), any number of spaces, and a double quote (")



(FILENAME != TABLE_FILENAME) {
  # Make sure this is a viable candidate symbol line for processing... if not, just skip
  #  this line.
  # Extract the literal name
  FOUNDFLAG = 0;
  if (substr($1,1,9)=="DCL_TEXT_") {
     lit_name = substr($1,5) ;
     FOUNDFLAG = 1;
  }
  if (substr($1,1,10)=="UDCL_TEXT_") {
     lit_name = substr($1,6) ;
     FOUNDFLAG = 1;
     INUPDFLAG = 1;
  }
  if (substr($1,2,9)== "DCL_TEXT_") {
     lit_name = substr($1,6) ;
     FOUNDFLAG = 1;
     INUPDFLAG = 1;
  }
  if (substr($1,1,5)=="TEXT_") {
     lit_name = $1 ;
     FOUNDFLAG = 1;
  }
  if ($1=="char") {
     if (substr($2,1,9)=="DCL_TEXT_") {
        lit_name = substr($2,5) ;
        FOUNDFLAG = 1;
     }
     if (substr($2,1,10)=="UDCL_TEXT_") {
        lit_name = substr($2,6) ;
        FOUNDFLAG = 1;
        INUPDFLAG = 1;
     }
     if (substr($2,2,9)== "DCL_TEXT_") {
        lit_name = substr($2,6) ;
        FOUNDFLAG = 1 ;
        INUPDFLAG = 1 ;
     }
     if (substr($2,1,5)=="TEXT_") {
        lit_name = $2 ;
        FOUNDFLAG = 1;
     }
  }
  if ($1=="static") {
     if ($2 = "char") {
        if (substr($3,1,9)=="DCL_TEXT_") {
           lit_name = substr($3,5) ;
           FOUNDFLAG = 1;
        }
        if (substr($3,1,10)== "UDCL_TEXT_") {
           lit_name = substr($3,6) ;
           FOUNDFLAG = 1;
           INUPDFLAG = 1;
        }
        if (substr($3,1,5)=="TEXT_") {
           lit_name = $3 ;
           FOUNDFLAG = 1 ;
        }
        if (substr($3,2,9)== "DCL_TEXT_") {
           lit_name = substr($3,6) ;
           FOUNDFLAG = 1 ;
           INUPDFLAG = 1 ;
        }
     }
  }
  if (FOUNDFLAG == 0) {

#  if (index($0, "TEXT_") != 0) {
     printf("// Non-TEXT line = %s\n", $0);
     next;
  }

# printf("@%s@\n",$0);

#########################################################################
# Now insert the appropriate header depending on what section we are in
#########################################################################

  if ((FOUNDFLAG==1)&&(INUPDFLAG==0)&&(SECTIONID!=1)) {
#    This means we are in the initial section of new strings and 
#    we haven't put the header in yet.

     if (SECTIONID!=0) {
        printf("<<>>\n");
        printf("<<SECTION END>>\n");
     }
     printf("<<>>\n");
     printf("<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
     printf("<<NEW STRING>>\n");
     printf("<<>>\n");
     printf("\n");
     SECTIONID=1;
  }

  if ((FOUNDFLAG==1)&&(INUPDFLAG==1)&&(SECTIONID!=2)) {
# In new string section

     if (SECTIONID!=0) {
        printf("<<>>\n");
        printf("<<SECTION END>>\n");
     }

     printf("<<>>\n");
     printf("<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
     printf("<<UPDATED STRING>>\n");
     printf("<<>>\n");
     printf("\n");
     SECTIONID=2;
  }

  # This will burn the trailing []= if necessary 
  lit_name2 = lit_name ;
  if (length(lit_name) > 0) {

     for(i = length(lit_name); i > 0; i--) {

        if ((substr(lit_name, i, 1) != "=") && 
            (substr(lit_name, i, 1) != "[") && 
            (substr(lit_name, i, 1) != "]")) {
           lit_name2 = substr(lit_name, 1, i) ;
           break;
        }
     } # End of for(i = leng...

  } # End of if (length(lit_name...

  lit_name = lit_name2;

  # Determine the number of blanks needed to align the language literal from the char decl.
  blanklen = 60 - (length("static char FDCL_ [] = ") + length(lit_name)) ;

  # Build the blanks padding string for the language literal alignment.
  if (blanklen < 0) {
     blanklen = 0;
  }

  blankpads = "" ;

  while(blanklen > 0) {
      blankpads = blankpads " " ;
      blanklen--;
  }

  # Find the position of the language values
  ilanpos = match($0,"=[[:space:]]*\"") ;
#  tlanguage1 = substr(substr($0,ilanpos+1),match(substr($0,ilanpos+1),"\"")+1) ;
#  tlanguage = substr(tlanguage1,1,match(tlanguage1,"\"")-1); 
#  language = "";
   language = substr(substr($0,ilanpos+1),match(substr($0,ilanpos+1),"\"")) ;
#
#  tlanglen = length(tlanguage) ;

#  for (indx=1; indx<=tlanglen; indx++) {

#     if(substr(tlanguage,indx,1) == "\"" || substr(tlanguage,indx,1) == "\011") {
#        # Do nothing, don't copy quotes or tabs across
#     }
#     else { # Copy something across
#        if(substr(tlanguage,indx,1) == "\205") {
#             # Replace ILE's non-standard ellipses
#             language = language "..." ;
#        }
#        else {
#           # Copy original source char.
#           language = language substr(tlanguage,indx,1) ;
#        }

#     }

#  } # End of "for (indx=1; indx<=tlanglen...


  # This will burn any leading blanks
#  while(length(language) > 0 && substr(language, 1, 1) == " ") {
#
#         if (length(language) > 1) {
#            language = substr(language, 2) ;
#         }
#         else {
#            language = "" ;
#         }

#  } # End of while(length(lang...

  # This will burn any trailing blanks and semicolon
#  language2 = language;
#  language2 = " ";
#  if (length(language) > 0) {
#
#     for(i = length(language); i > 0; i--) {
#        if (substr(language, i, 1) != " ") { 
#           language2 = substr(language, 1, i) ;
#           break;
#        }
#     } # End of for(i = leng...
#
#  } # End of if (length(langaug...

#  language = language2 ;


  # Store this language element in the cross check array (even if a null string)
  store_elements(lit_name, language, LANG) ;


  # Print the common declaration prefix
  printf ("static char %sDCL_%s [] = %s \n", LANG, lit_name, language);

  # Print out the language literal after the decl. prefix.
#  printf("%s\"%s\" ;\n", blankpads, language);
#  printf("\"%s\" ;\n",language);
  # Add to the checksum
  acc = (acc + (NF * NR * length($0))) % CSDIVISOR;





} ##### END of ILE translation file processing block





# This section of the Focussed Translation to C++ conversion
#  takes the English translat.h  file (which is the arbiter of
#  updates to the text interface) and converts it
#  to an update file for the target language. It does
#  this by copying the entire array and converting the literal
#  name prefix to that of the target language's literal
#  prefix, e.g. DCL_ --> FDCL_ (for French).


# This code manages changes from section to section The only sections 
# that require processing are NEW_STYLE and UPDATE_STYLE
((FILENAME==TABLE_FILENAME)&&(SECTIONID==0)) {
  if ( (index($0,"ifdef") >0 ) && (index($0,"NEW_STYLE") >0 ) ) {
     SECTIONID=3;
     printf("<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
     printf("<<>>\n");
     printf("<<NEW STYLING>>\n");
     printf("<<>>\n");
  }
  else if ( (index($0,"ifdef") >0 ) && (index($0,"UPDATE_STYLE") >0 ) ) {
     SECTIONID=4;
     printf("<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
     printf("<<>>\n");
     printf("<<UPDATED STYLING>>\n");
     printf("<<>>\n");
  }
  getline;
}

# This code determines if we are in a comment area so that comments that include
# #include or #define strings are not parsed in the subsequent section
# This is primarily to handle the header area which contains such comments
(FILENAME == TABLE_FILENAME) {
  if (incomment_flag==0) {
    if (substr($1,1,2)=="/*") {
       incomment_flag=1 ;
    }
  }
  if (incomment_flag==1) {
    if ( index($0,"*/") >0 ) {
       incomment_flag=0 ;
    }
  }
}

# This code looks for transitions out of a particular section.
((FILENAME == TABLE_FILENAME) && (SECTIONID!=0)) {
  if (index($0,"endif") >0 ) {
     SECTIONID=0;
     printf("<<>>\n");
     printf("<<SECTION END>>\n");
     printf("<<>>\n");
     getline;
  }

   # Record whether this styling element has a text ID literal (or not) and
   #  make a record of this in its respective array.

   if ((index($1,"TEXT\_") == 1) && (index($0,"txt_id") > 0)) {
     # printf("OLD SYMBOL = <%s>\n", $1);
      new_symbol = strip_symbol($1);
     # printf("NEW SYMBOL = <%s>\n", new_symbol);

      if (new_symbol in language_literal_array) {
         litreferenced_elements(new_symbol);
      }
      else {
         litnotfound_elements(new_symbol);
      }

   } # End of if "(index($1,"TEXT_"...



   # This code translates and writes the style table into the target language.
   if (SECTIONID==3) {
      num_reped  = gsub("DCL\_", new_prefix, $0);
   }
   if (SECTIONID==4) {
      num_reped = gsub("UDCL\_", new_prefix, $0);
   }
   print $0 ;

   # Add to the stringtb checksum
   accstr = (accstr + (NF * NR * length($0))) % CSDIVISOR;
}


END {

   printf("\n\n// TRANSLATION SOURCE FILE CHECKSUM = %d\n", acc);
   printf("// STRINGTB.CPP FILE CHECKSUM       = %d\n", accstr);


   # Print out all Literals with no styling record reference.
   printf("\n\n// LITERALS WITH NO REFERENCING STYLING RECORD\n\n");


   # Get rid of all the referenced literal symbols leaving only the
   #  unreferenced symbols.
   for (ALLITS in language_litrefed_array) {
      delete language_literal_array[ALLITS];
   }

   # Now print all the remaining "unreferenced symbols" in the original
   #  literal symbols table.
   for (ALLITSX in language_literal_array) {
      printf("//\t\t%s\n", ALLITSX);
   }  # End of "for (ALLITS in langua...




   # Print out all the "NOT FOUND" referenced literals
   printf("\n\n\n// LITERALS REFERENCED BY STYLING RECORDS BUT NOT DEFINED\n\n");
   for (INXN in language_nolit_array) {
      printf("//static char %sDCL\_%s [] = \"%s\" ;\n", LANG, INXN, "UNKNOWN LIT.");
   }

} # End of the END procedure

