call 1234h
call dword 50h
call ax
call [bp]
call 1234h:5678h
call 1234h:5678ABCDh
call 1234h:123456789ABCh
call dword 1234h:56789ABCh
call 1234h:dword 56789ABCh
call [bx+si+34]
call far [bx+si+34]
call dword far [eax-12]
call far dword [eax-12]
