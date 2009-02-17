

generatedirs()
{
	if [ -z "$1" ] ; then return ; fi;
	for i in share apps config foo bar $2
	do
		mkdir -p  $1/$i
	done
}

fillfiles()
{
	if [ -z "$1" ] ; then return ; fi;
	generatedirs $@
	for i in share apps config foo bar $2
	do
		for j in mail web magic $3
		do
			echo $1 > $1/$i/$j
		done
	done


}



fillfiles kde quux kdespecific
fillfiles kde4

