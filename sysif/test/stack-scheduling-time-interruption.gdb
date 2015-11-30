# -*- mode: gdb-script -*-

set verbose off
set confirm off

set $var_user_process_1=0
set $var_user_process_2=0
set $var_user_process_3=0

break user_process_1
commands
  set $var_user_process_1++
  print $var_user_process_1
  continue 
end

break user_process_2
commands
  set $var_user_process_2++
  print $var_user_process_2
  continue 
end

break user_process_3
commands
  set $var_user_process_3++
  print $var_user_process_3
  continue 
end

break sys_yield
commands
  if $var_user_process_3==2
      assess_execution
  end
  continue 
end

define assess_execution
  # integer used as boolean, multiplication used as logical AND
  set $ok = 1
  # check that no two stacks share the same location
  set $ok *= ($var_user_process_1 == $var_user_process_2 && $var_user_process_1 == $var_user_process_3 && $var_user_process_1 == 2) 

  if $ok
    printf "test OK\n"
  else
    printf "test ERROR\n"
  end
  quit
end

target remote:1234
continue
