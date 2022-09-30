if exists("b:current_syntax")
    finish
endif

syntax keyword kal_type num str
syntax keyword kal_todo TODO NOTE FIXME README
syntax keyword kal_keyword var del const exit warn read list push join size write throw stdin stdout stderr concat style

syntax match kal_comment ";.*$" contains=kal_todo
syntax match kal_shebang "#!.*$" 

syntax region kal_string start='"' end='"'

highlight kal_keyword     ctermfg=cyan
highlight kal_todo        ctermfg=white    ctermbg=gray    cterm=bold
highlight kal_comment     ctermfg=gray                     cterm=italic
highlight kal_string      ctermfg=green
highlight kal_shebang     ctermfg=red                      cterm=bold
highlight kal_type        ctermfg=142                      cterm=italic
