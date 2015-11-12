let SessionLoad = 1
if &cp | set nocp | endif
let s:so_save = &so | let s:siso_save = &siso | set so=0 siso=0
let v:this_session=expand("<sfile>:p")
silent only
cd /tmp/jim-db
if expand('%') == '' && !&modified && line('$') <= 1 && getline(1) == ''
  let s:wipebuf = bufnr('%')
endif
set shortmess=aoO
badd +47 src/main.cpp
badd +90 src/page/page.h
badd +260 src/page/page.cpp
badd +25 src/network/tcpserver.h
badd +1 src/network/tcpserver.cpp
badd +1 src/network/clienthandle.cpp
badd +36 src/network/clienthandle.h
badd +1 .ycm_extra_conf.py
badd +25 buildenvironment/posix.py
badd +1 .git/refs/heads/master
badd +1 src/page
badd +1 /usr/include/c++/5/bits/stl_vector.h
badd +1 src/network/message.h
badd +253 /usr/include/c++/4.9/bits/alloc_traits.h
badd +1090 /usr/include/c++/4.9/bits/shared_ptr_base.h
badd +124 /usr/include/c++/4.9/ext/new_allocator.h
badd +316 /usr/include/c++/4.9/bits/shared_ptr.h
badd +40 src/network/message.hpp
badd +1858 library/build/rapidjson/1.0.2/include/rapidjson/document.h
badd +1 /tmp/jim-db
badd +30 src/tasking/inserttask.cpp
badd +34 src/tasking/inserttask.h
badd +1 library/build/rapidjson/1.0.2/include
badd +38 src/meta/metaindex.h
badd +96 src/index/index.h
badd +1 src/index/index.c
badd +0 library/build/btree/0.9/include/stx/btree_map.h
silent! argdel *
edit src/main.cpp
set splitbelow splitright
wincmd _ | wincmd |
split
wincmd _ | wincmd |
split
2wincmd k
wincmd w
wincmd w
set nosplitbelow
set nosplitright
wincmd t
set winheight=1 winwidth=1
exe '1resize ' . ((&lines * 3 + 21) / 43)
exe '2resize ' . ((&lines * 25 + 21) / 43)
exe '3resize ' . ((&lines * 10 + 21) / 43)
argglobal
enew
setlocal fdm=manual
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=0
setlocal fml=1
setlocal fdn=20
setlocal fen
wincmd w
argglobal
setlocal fdm=manual
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=0
setlocal fml=1
setlocal fdn=20
setlocal fen
silent! normal! zE
let s:l = 114 - ((2 * winheight(0) + 12) / 25)
if s:l < 1 | let s:l = 1 | endif
exe s:l
normal! zt
114
normal! 05|
wincmd w
argglobal
enew
setlocal fdm=manual
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=0
setlocal fml=1
setlocal fdn=20
setlocal fen
wincmd w
exe '1resize ' . ((&lines * 3 + 21) / 43)
exe '2resize ' . ((&lines * 25 + 21) / 43)
exe '3resize ' . ((&lines * 10 + 21) / 43)
tabedit .ycm_extra_conf.py
set splitbelow splitright
set nosplitbelow
set nosplitright
wincmd t
set winheight=1 winwidth=1
argglobal
setlocal fdm=manual
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=0
setlocal fml=1
setlocal fdn=20
setlocal fen
silent! normal! zE
let s:l = 50 - ((3 * winheight(0) + 20) / 40)
if s:l < 1 | let s:l = 1 | endif
exe s:l
normal! zt
50
normal! 032|
tabedit library/build/btree/0.9/include/stx/btree_map.h
set splitbelow splitright
wincmd _ | wincmd |
split
1wincmd k
wincmd w
set nosplitbelow
set nosplitright
wincmd t
set winheight=1 winwidth=1
exe '1resize ' . ((&lines * 29 + 21) / 43)
exe '2resize ' . ((&lines * 10 + 21) / 43)
argglobal
setlocal fdm=manual
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=0
setlocal fml=1
setlocal fdn=20
setlocal fen
silent! normal! zE
let s:l = 1 - ((0 * winheight(0) + 14) / 29)
if s:l < 1 | let s:l = 1 | endif
exe s:l
normal! zt
1
normal! 0
lcd /tmp/jim-db
wincmd w
argglobal
enew
setlocal fdm=manual
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=0
setlocal fml=1
setlocal fdn=20
setlocal fen
lcd /tmp/jim-db
wincmd w
exe '1resize ' . ((&lines * 29 + 21) / 43)
exe '2resize ' . ((&lines * 10 + 21) / 43)
tabedit /usr/include/c++/5/bits/stl_vector.h
set splitbelow splitright
wincmd _ | wincmd |
split
1wincmd k
wincmd w
set nosplitbelow
set nosplitright
wincmd t
set winheight=1 winwidth=1
exe '1resize ' . ((&lines * 1 + 21) / 43)
exe '2resize ' . ((&lines * 38 + 21) / 43)
argglobal
enew
setlocal fdm=manual
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=0
setlocal fml=1
setlocal fdn=20
setlocal fen
lcd /tmp/jim-db
wincmd w
argglobal
setlocal fdm=manual
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=0
setlocal fml=1
setlocal fdn=20
setlocal fen
silent! normal! zE
let s:l = 1 - ((0 * winheight(0) + 19) / 38)
if s:l < 1 | let s:l = 1 | endif
exe s:l
normal! zt
1
normal! 0
lcd /tmp/jim-db
wincmd w
exe '1resize ' . ((&lines * 1 + 21) / 43)
exe '2resize ' . ((&lines * 38 + 21) / 43)
tabedit /tmp/jim-db/buildenvironment/posix.py
set splitbelow splitright
set nosplitbelow
set nosplitright
wincmd t
set winheight=1 winwidth=1
argglobal
setlocal fdm=manual
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=0
setlocal fml=1
setlocal fdn=20
setlocal fen
silent! normal! zE
let s:l = 44 - ((16 * winheight(0) + 15) / 31)
if s:l < 1 | let s:l = 1 | endif
exe s:l
normal! zt
44
normal! 047|
lcd /tmp/jim-db
tabnext 3
if exists('s:wipebuf')
  silent exe 'bwipe ' . s:wipebuf
endif
unlet! s:wipebuf
set winheight=1 winwidth=20 shortmess=filnxtToO
let s:sx = expand("<sfile>:p:r")."x.vim"
if file_readable(s:sx)
  exe "source " . fnameescape(s:sx)
endif
let &so = s:so_save | let &siso = s:siso_save
doautoall SessionLoadPost
let g:this_obsession = v:this_session
unlet SessionLoad
" vim: set ft=vim :
