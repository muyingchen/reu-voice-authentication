# "v_edit.awk" by R. J. Onyshczak - Dec 20, 1990
#
# this awk program verifys that a sentence has a valid ATIS id,
# gets rid of "(n)", where n is a digit, and converts "(n verbage)" to simply
# "verbage".
#
# copy all fields to elements of "field".
#
{for (i = 1; i <= NF; i++)
    field[i] = $i;
#
# clear idok flag.
#
idok = 0;
#
# if the last field starts with an open paren, set the idok flag.
#
if(substr(field[NF], 1, 1) == "(")
	idok = 1;
#
# if the next eight characters in last field are alphanumeric, set the idok
# flag.  Otherwise, clear it.
#
for(i = 2;i <= 9; i++)
{
	if(((substr(field[NF], i, 1) >= "0") && \
	    (substr(field[NF], i, 1) <= "9")) || \
	   ((substr(field[NF], i, 1) >= "a") && \
	    (substr(field[NF], i, 1) <= "z")) || \
	   ((substr(field[NF], i, 1) >= "A") && \
 	    (substr(field[NF], i, 1) <= "Z"))) 
		idok = 1;
	else
		idok = 0;
}
#
# If the id_ok flag is set and 10'th character is not close paren, clear the
# the idok flag.
#
if((idok == 1) && (substr(field[NF], 10, 1) != ")"))
	idok = 0;
#
# if there is a valid ATIS id on the end of the sentence, set number of fields
# to NF-1, otherwise set it to NF.
#
nfields = NF - idok;
#
# clear the begin flag.
#
begin = 0;
#
# for each of the valid fields,
#
for(i = 1; i <= nfields; i++)
{
#
# if we have not found only an open paren and a digit,
#
	if(begin == 0)
	{
#
# look for an open paren as the first character, and a digit as the second
# character.  If we find this combination,
#
		if((substr($i, 1, 1) == "(") && \
		   (substr($i, 2, 1) >= "0") && \
		   (substr($i, 2, 1) <= "9"))
		{
#
# if the length of this field is 2, set the begin flag.
#
			if(length($i) == 2)
				begin = 1;
#
# set this field to null.
#
			field[i] = "";
		}
	}
#
# if we have found only an open paren and a digit, look for a field ending
# in a close paren.
#
	else
	{
#
# When we find one, copy everything except the last character to field
# variable and clear the begin flag.
#
		if(substr($i, length($i), 1) == ")")
		{
			field[i] = substr($i, 1, length($i)-1);
			begin = 0;
		}
	}
#
# print out the current field.
#
	printf("%s ", field[i]);
}
#
# if there is a valid atis id, print it as the last field on the line.
# In all cases, output a new line character.
#
if(idok == 1) printf("%s", field[NF]);
printf("\n");
}
