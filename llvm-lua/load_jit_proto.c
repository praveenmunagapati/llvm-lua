/*

  Copyright (c) 2008 Robert G. Jakabosky
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  
  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.
  
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.

  MIT License: http://www.opensource.org/licenses/mit-license.php
*/


#include <string.h>
#include "lua_core.h"
#include "lobject.h"
#include "lfunc.h"
#include "ldo.h"
#include "lstring.h"
#include "lmem.h"
#include "load_jit_proto.h"

Proto *load_jit_proto(lua_State *L, jit_proto *p) {
	Proto *f = luaF_newproto(L);
	int i;

	/* proto source */
	f->source = luaS_new(L, p->name);
	/* jit_func */
	f->jit_func = p->jit_func;
	/* nups */
	f->nups = p->nups;
	/* numparams */
	f->numparams = p->numparams;
	/* is_vararg */
	f->is_vararg = p->is_vararg;
	/* maxstacksize */
	f->maxstacksize = p->maxstacksize;
	/* sizek */
	f->sizek = p->sizek;
	/* k */
	f->k=luaM_newvector(L,p->sizek,TValue);
	for(i = 0; i < p->sizek; i++) {
		TValue *o=&f->k[i];
		switch(get_constant_type(p->k[i].type_length)) {
			case TYPE_STRING:
				setsvalue2n(L,o, luaS_newlstr(L, p->k[i].val.str,
					get_constant_length(p->k[i].type_length)));
				break;
			case TYPE_BOOLEAN:
				setbvalue(o, p->k[i].val.b != 0);
				break;
			case TYPE_NUMBER:
				setnvalue(o, p->k[i].val.num);
				break;
			case TYPE_NIL:
			default:
				setnilvalue(o);
				break;
		}
	}
	/* sizep */
	f->sizep = p->sizep;
	/* p */
	f->p=luaM_newvector(L,p->sizep,Proto*);
	for(i = 0; i < p->sizep; i++) {
		f->p[i] = load_jit_proto(L, &(p->p[i]));
	}
	/* sizecode */
	f->sizecode = p->sizecode;
	/* code */
	f->code=luaM_newvector(L,p->sizecode,Instruction);
	for(i = 0; i < p->sizecode; i++) {
		f->code[i] = p->code[i];
	}
	return f;
}
