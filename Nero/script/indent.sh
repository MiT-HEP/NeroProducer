#!/bin/bash

cpp=$(find . -type f | grep '\.hpp$\|\.cpp$\|\.cxx$\|\.h$\|\.c$')
python=$(find . -type f | grep '\.py$')

cppstring="\n// Local Variables:\n// mode:c++\n// indent-tabs-mode:nil\n// tab-width:4\n// c-basic-offset:4\n// End:\n// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4\n"
pystring="\n# Local Variables:\n# mode:python\n# indent-tabs-mode:nil\n# tab-width:4\n# c-basic-offset:4\n# End:\n# vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4\n"

function append
{
	## append on mode=$1 filename=$2 the string
	## mode = 'cpp' or "py"
	mode=$1
	filename=$2

	num=$(tail -n 10  $2 | grep -A 6 'Local Variables:'  | wc -l )
	if [ $num -eq 0 ] ; then
		[ "$mode" == "cpp" ] && echo -n -e $cppstring >> $filename
		[ "$mode" == "py" ] && echo -n -e $pystring >> $filename
	fi
}

function indent
{
	filename=$1
	$HOME/astyle/astyle/build/gcc/bin/astyle --options=$HOME/astyle/astyle/build/gcc/bin/options.ast -q $filename
}

function vimindent
{
	filename=$1
	#vim -c "let @l='Aj' " -c "normal! 1000@l" -c ":x" $filename
	vim -c "let @l='Aj' " -c "%normal @l" -c ":x" $filename
}

function sequence
{
	mode=$1
	filename=$2
	
	echo "## Doing append $mode $filename##"
	append $mode $filename;
	#echo "## Doing indent $mode $filename##"
	#indent $filename;
	echo "## Doing vimindent $filename##"
	vimindent $filename;
	echo "## Done $filename ##"
}


for file in $cpp; do sequence 'cpp' "$file" ; done
for file in $python ; do sequence 'py' "$file" ; done
