" Vim syntax file
" Language: Lwanga
" Maintainer: Lwanga Language Team
" Latest Revision: 2024

if exists("b:current_syntax")
  finish
endif

" Keywords
syn keyword lwangaKeyword fn let mut const struct import as packed return
syn keyword lwangaConditional if else while
syn keyword lwangaUnsafe unsafe
syn keyword lwangaAsm asm naked
syn keyword lwangaSyscall syscall
syn keyword lwangaEnc enc

" Types
syn keyword lwangaType u8 u16 u32 u64 ptr register

" Constants
syn keyword lwangaConstant true false null

" Operators
syn match lwangaOperator "\v\+|-|\*|/|%"
syn match lwangaOperator "\v\&|\||\^|~|<<|>>"
syn match lwangaOperator "\v\=\=|!\=|\<|\>|\<\=|\>\="
syn match lwangaOperator "\v\&\&|\|\||!"
syn match lwangaOperator "\v\="
syn match lwangaOperator "\v-\>"

" Numbers
syn match lwangaNumber "\v<0x[0-9a-fA-F]+>"
syn match lwangaNumber "\v<0b[01]+>"
syn match lwangaNumber "\v<\d+>"

" Strings
syn region lwangaString start='"' end='"' contains=lwangaEscape
syn match lwangaEscape "\\[nrt\\0\"]" contained
syn match lwangaEscape "\\x[0-9a-fA-F]\{2}" contained

" Comments
syn keyword lwangaTodo TODO FIXME XXX NOTE contained
syn match lwangaComment "//.*$" contains=lwangaTodo
syn region lwangaComment start="/\*" end="\*/" contains=lwangaTodo

" Preprocessor
syn match lwangaPreProc "^\s*#\(if\|else\|endif\|define\)\>"

" Functions
syn match lwangaFunction "\v<\w+>\s*\ze\("

" Regions
syn region lwangaBlock start="{" end="}" transparent fold

" Highlighting
hi def link lwangaKeyword Keyword
hi def link lwangaConditional Conditional
hi def link lwangaUnsafe Error
hi def link lwangaAsm Special
hi def link lwangaSyscall Special
hi def link lwangaEnc Special
hi def link lwangaType Type
hi def link lwangaConstant Constant
hi def link lwangaOperator Operator
hi def link lwangaNumber Number
hi def link lwangaString String
hi def link lwangaEscape SpecialChar
hi def link lwangaComment Comment
hi def link lwangaTodo Todo
hi def link lwangaPreProc PreProc
hi def link lwangaFunction Function

let b:current_syntax = "lwanga"
