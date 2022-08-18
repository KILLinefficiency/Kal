if exists("b:current_syntax")
	finish
endif

syntax keyword kal_keyword var const stdin stdout stderr
syntax keyword kal_todo TODO NOTE FIXME README

syntax match kal_comment ";.*$" contains=kal_todo
syntax match kal_shebang "#!.*$" 

syntax region kal_string start='"' end='"'

highlight kal_keyword     ctermfg=yellow
highlight kal_todo        ctermfg=yellow
highlight kal_comment     ctermfg=gray
highlight kal_string      ctermfg=green
highlight kal_shebang     ctermfg=red
