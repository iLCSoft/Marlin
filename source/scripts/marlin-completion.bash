#/usr/bin/env bash

marlin_completion() {
  if ! [ -x "$(command -v MarlinShellCompletion)" ]; then
    return
  fi
  xmlfile=""
  current_id=0
  comp_marlin=""
  for arg in $COMP_LINE; do
    # echo "Current id is $current_id"
    if [ $current_id == 0 ]; then
      # echo "Skipping arg no $current_id"
      ((current_id++))
      continue
    fi
    if [ "${arg: -4}" == ".xml" ] && [[ $arg != --* ]]; then
      xmlfile=$arg
      # echo "Arg no $current_id is XML file"
      ((current_id++))
      continue
    fi
    if [ "$arg" != "${COMP_WORDS[${COMP_CWORD}]}" ] && [[ $arg == --* ]]; then
      comp_marlin="$comp_marlin $arg"
    fi
    ((current_id++))
  done
  if [ -z "$xmlfile" ]; then
    return
  fi
  # echo "XML file is $xmlfile"
  opts=`MarlinShellCompletion $xmlfile $comp_marlin`
  ret=$?
  if ! [ $ret == 0 ]; then
    echo ""
    echo "<invalid steering file>"
    return 
  fi
  opts=`echo $opts | tr '\n' ' '`  
  COMPREPLY=($(compgen -W '$opts' -- "${COMP_WORDS[${COMP_CWORD}]}"))
}

complete -o default -o nospace -F marlin_completion Marlin
complete -o default -o nospace -F marlin_completion MarlinMT