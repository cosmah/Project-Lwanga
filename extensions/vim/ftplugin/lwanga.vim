" Vim filetype plugin file
" Language: Lwanga

if exists("b:did_ftplugin")
  finish
endif
let b:did_ftplugin = 1

" Comment settings
setlocal commentstring=//\ %s
setlocal comments=://,s1:/*,mb:*,ex:*/

" Indentation
setlocal autoindent
setlocal smartindent
setlocal cindent
setlocal cinoptions=:0,l1,g0,t0,(0,W4

" Formatting
setlocal formatoptions-=t formatoptions+=croql

" Folding
setlocal foldmethod=syntax
setlocal foldlevel=99

" Compiler
if executable('lwangac')
  setlocal makeprg=lwangac\ %\ -o\ %:r
endif

" Format command
if executable('lwangafmt')
  command! -buffer LwangaFormat execute '!lwangafmt --write %'
  command! -buffer LwangaAddHeader execute '!lwangafmt --add-header %'
endif

" Compile command
if executable('lwangac')
  command! -buffer LwangaCompile execute '!lwangac % -o %:r'
  command! -buffer LwangaCompileOptimized execute '!lwangac -O2 % -o %:r'
endif

" Keybindings
nnoremap <buffer> <leader>lf :LwangaFormat<CR>
nnoremap <buffer> <leader>lh :LwangaAddHeader<CR>
nnoremap <buffer> <leader>lc :LwangaCompile<CR>
nnoremap <buffer> <leader>lo :LwangaCompileOptimized<CR>

let b:undo_ftplugin = "setlocal commentstring< comments< autoindent< smartindent< cindent< cinoptions< formatoptions< foldmethod< foldlevel< makeprg<"
