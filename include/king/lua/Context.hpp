//lua c api �ķ��b
#ifndef KING_LIB_HEADER_LUA_CONTEXT
#define KING_LIB_HEADER_LUA_CONTEXT

#include <lua/lua.hpp>

#include <iostream>
#include <string>
#include <memory>

namespace king
{
	namespace lua
	{
		typedef std::shared_ptr<lua_State> LuaSpt;

		//lua ���\�� �h��
		class Context
		{
		protected:
			LuaSpt _l;
			static void D(lua_State* l)
			{

			}
		public:
			explicit Context(bool openlibs = true)
			{
				lua_State* l = luaL_newstate();
				if(l)
				{
					_l = LuaSpt(l,lua_close);
					if(openlibs)
					{
						luaL_openlibs(l);
					}
				}
			}
			explicit Context(lua_State* l,bool close = true/*�Ƿ��������r ጷ�lua�h��*/)
			{
				if(close)
				{
					_l = LuaSpt(l,lua_close);
				}
				else
				{
					_l = LuaSpt(l,D);
				}
			}
			//copy
			explicit Context(const LuaSpt& l)
			{
				_l = l;
			}
			explicit Context(const Context& copy)
			{
				_l = copy._l;
			}
			inline Context& operator=(const Context& copy)
			{
				_l = copy._l;
				return *this;
			}
			//move
			Context(LuaSpt&& l)
			{
			    _l.swap(l);
			}
			Context(Context&& c)
			{
				_l.swap(c._l);
			}
			inline Context& operator=(Context&& c)
			{
				_l = c._l;
				c.Reset();
				return *this;
			}
			inline void Reset()
			{
			    _l.reset();
			}
			inline void Swap(Context& c)
			{
			    _l.swap(c._l);
			}

			explicit inline operator bool()const
			{
			    return _l?true:false;
			}
            inline operator LuaSpt()const
			{
				return _l;
			}
			inline operator lua_State*()const
			{
				return _l.get();
			}
			inline LuaSpt GetLuaSpt() const
			{
				return _l;
			}
			inline lua_State* Get()const
			{
				return _l.get();
			}


		public:
			/******	�Uչ api	******/
			//�� ��ǰ �� ��ӡ���˜�ݔ��
			void dump_stdout()
			{
				int n = lua_gettop(_l.get());
				printf("ctx: top=%d, stack=[",n);
				for(int i=0;i<n;++i)
				{
					if(i)
					{
						printf(",");
					}
					dump_stdout(i+1);
				}
				puts("]");
			}
		protected:
			void dump_stdout(int n)
			{
				lua_State* l = _l.get();

				switch(lua_type(l,n))
				{
				case LUA_TNONE :
					std::cout<<"none";
					break;
				case LUA_TNIL:
					std::cout<<"nil";
					break;
				case LUA_TBOOLEAN:
					if(lua_toboolean(l,n))
					{
						std::cout<<"true";
					}
					else
					{
						std::cout<<"false";
					}
					break;
				case LUA_TLIGHTUSERDATA:
					std::cout<<"LUA_TLIGHTUSERDATA";
					break;

				case LUA_TNUMBER:
					lua_pushvalue(l,n);
					std::cout<<lua_tostring(l,-1);
					lua_pop(l,1);
					break;
				case LUA_TSTRING:
					std::cout<<"\""<<lua_tostring(l,n)<<"\"";
					break;
				case LUA_TTABLE:
					{
						std::cout<<"{";
						lua_pushnil(l);
						bool first = true;
						while(lua_next(l,n))
						{
							//�� ���Q �}�uһ�� ���⌢ index �D�Q�� string
							lua_pushvalue(l, -2);
							if(first)
							{
								first = false;
							}
							else
							{
								std::cout<<",";
							}
							bool is_str = lua_type(l,-1) == LUA_TSTRING;
							if(is_str)
							{
								std::cout<<"\"";
							}
							std::cout<<lua_tostring(l,-1);
							if(is_str)
							{
								std::cout<<"\"";
							}
							std::cout<<":";

							dump_stdout(-2);

							lua_pop(l, 2);
						}
						std::cout<<"}";
					}
					break;
				case LUA_TFUNCTION:
					std::cout<<"LUA_TFUNCTION";
					break;
				case LUA_TUSERDATA:
					std::cout<<"LUA_TUSERDATA";
					break;
				case LUA_TTHREAD:
					std::cout<<"LUA_TTHREAD";
					break;
				case LUA_NUMTAGS:
					std::cout<<"LUA_NUMTAGS";
					break;
				default:
					std::cout<<"unknow";
					break;
				}
			}


		public:
			/******	lua c api ����	******/

			/***	�� ����	***/
			//����
			inline void pop(int n=1)
			{
				lua_pop(_l.get(),n);
			}

			//�뗣
			inline void push_boolean(int b)
			{
				lua_pushboolean(_l.get(),b);
			}
			inline void push_boolean(bool b)
			{
				if(b)
				{
					lua_pushboolean(_l.get(),1);
				}
				else
				{
					lua_pushboolean(_l.get(),0);
				}

			}
			inline void push(lua_CFunction fn, int n)
			{
				lua_pushcclosure(_l.get(),fn,n);
			}
			inline void push(lua_CFunction fn)
			{
				lua_pushcfunction(_l.get(),fn);
			}
			inline void push_globaltable()
			{
				lua_pushglobaltable(_l.get());
			}
			inline void push_integer(lua_Integer n)
			{
				lua_pushinteger(_l.get(),n);
			}
			inline void push_lightuserdata (void *p)
			{
				lua_pushlightuserdata(_l.get(),p);
			}
			inline void push_nil()
			{
				lua_pushnil(_l.get());
			}
			inline void push_number(lua_Number n)
			{
				lua_pushnumber(_l.get(),n);
			}
			inline void push(const char *s)
			{
				lua_pushstring(_l.get(),s);
			}
			inline void push(const char *s,std::size_t len)
			{
				lua_pushlstring(_l.get(),s,len);
			}
			inline void push(const std::string& s)
			{
				lua_pushlstring(_l.get(),s.data(),s.size());
			}
			inline void push_thread()
			{
				lua_pushthread(_l.get());

			}
			//�}�u һ�ݔ������� �K�뗣
			inline void push_value(std::size_t idx)
			{
				lua_pushvalue(_l.get(),idx);
			}

			//�@ȡ ȫ��׃��/nil ����� ���� ��lua�̈́e
			inline int get_global(const char* name)
			{
				return lua_getglobal(_l.get(),name);
			}
			inline int get_global(const std::string& name)
			{
				return lua_getglobal(_l.get(),name.c_str());
			}
			//�� ��� �O�Þ� ȫ��׃�� �K����
			inline void set_global(const char* name)
			{
				lua_setglobal(_l.get(),name);
			}
			inline void set_global(const std::string& name)
			{
				lua_getglobal(_l.get(),name.c_str());
			}

			//check
			inline bool check_stack(int sz)
			{
				return lua_checkstack(_l.get(),sz) != 0;
			}
			inline void l_check_stack(int sz, const char *msg=NULL)
			{
				luaL_checkstack(_l.get(),sz,msg);
			}

			/******	���ؔ���	******/
			inline int type(int idx)const
			{
			    return lua_type(_l.get(),idx);
			}
			inline bool is_function(int idx)const
			{
				return lua_isfunction(_l.get(),idx);
			}
			inline bool is_table(int idx)const
			{
				return lua_istable(_l.get(),idx);
			}
			inline bool is_lightuserdata(int idx)const
			{
				return lua_islightuserdata(_l.get(),idx);
			}
			inline bool is_nil(int idx)const
			{
				return lua_isnil(_l.get(),idx);
			}
			inline bool is_integer(int idx)const
			{
				return lua_isinteger(_l.get(),idx);
			}
			inline bool is_number(int idx)const
			{
				return lua_isnumber(_l.get(),idx);
			}
			inline bool is_boolean(int idx)const
			{
				return lua_isboolean(_l.get(),idx);
			}
			inline bool is_thread(int idx)const
			{
				return lua_isthread(_l.get(),idx);
			}
			inline bool is_none(int idx)const
			{
				return lua_isnone(_l.get(),idx);
			}

			inline bool to_boolean(int idx)
			{
				return lua_toboolean(_l.get(),idx) != 0;
			}
			inline lua_Integer to_integer(int idx)
			{
				return lua_tointeger(_l.get(),idx);
			}
			inline lua_Number to_tonumber(int idx)
			{
				return lua_tonumber(_l.get(),idx);
			}
			inline std::string to_string(int idx,size_t *len = NULL)
			{
				return lua_tolstring(_l.get(),idx,len);
			}
			inline const char* to_c_string(int idx,size_t *len = NULL)
			{
				return lua_tolstring(_l.get(),idx,len);
			}
			inline void* to_userdata(int idx)
			{
				return lua_touserdata(_l.get(),idx);
			}
			inline lua_State* to_thread(int idx)
			{
				return lua_tothread(_l.get(),idx);
			}
			inline lua_CFunction to_cfunction(int idx)
			{
				return lua_tocfunction(_l.get(),idx);
			}

			/***	table ����	***/

			//���� һ�� table �K�뗣
			//narr �AӋ ��ʹ�õ� ���M��С �ą���ֵ
			//nrec �AӋ ��ʹ�õ� hash���С �ą���ֵ
			inline void createtable(int narr=0, int nrec=0)
			{
				lua_createtable(_l.get(),narr,nrec);
			}

			//�� ��� �O�õ� table �� key �K����
			inline void put_prop_string(int idx/*table λ��*/,const char* key)
			{
				lua_setfield(_l.get(),idx,key);
			}
			inline void set_prop_string(int idx/*table λ��*/,const std::string& key)
			{
				lua_setfield(_l.get(),idx,key.c_str());
			}
			//�� ��� �O�õ� table �� ָ��λ�� �K����
			inline void set_prop_index(int idx/*table λ��*/,int arr_idx)
			{
				lua_seti(_l.get(),idx,arr_idx);
			}
			//��table�� ����key ���ص� ��� �����ڷ��� nil
			inline void get_prop_string(int idx/*table λ��*/,const std::string& key)
			{
				lua_getfield(_l.get(),idx,key.c_str());
			}
			//��table�� ����arr_idx ���ص� ��� �����ڷ��� nil
			inline void get_prop_index(int idx/*table λ��*/,int arr_idx)
			{
				lua_geti(_l.get(),idx,arr_idx);
			}



			/***	����� ����	***/
			//���d �ַ��� ����lua�_�� �K������������ ����
			inline bool l_loadstring(const char* s)
			{
				return luaL_loadstring(_l.get(),s) == LUA_OK;
			}
			inline bool l_loadstring(const std::string& s)
			{
				return luaL_loadstring(_l.get(),s.c_str()) == LUA_OK;
			}
			//���d �ļ� ����lua�_�� �K������������ ����
			inline int l_loadfile(const char* f)
			{
				return luaL_loadfile(_l.get(),f);
			}
			inline int l_loadfile(const std::string& f)
			{
				return luaL_loadfile(_l.get(),f.c_str());
			}

			/***	�{�� lua	***/
			/***	�� �뗣���� ֮�� ���� �������뗣 	***/
			inline void call(int nargs	/*��������*/ , int nresults/*����ֵ����*/)
			{
				lua_call(_l.get(),nargs,nresults);
			}
			inline bool pcall(int nargs	/*��������*/ , int nresults/*����ֵ����*/,int errfunc = 0/*���е�һ�� �e�`���{����*/)
			{
				return lua_pcall(_l.get(),nargs,nresults,errfunc) == LUA_OK;
			}
		};

		//lua �� scoped �\�� �h�� �����r���� �{�� lua_close
		//���� ���� lua c�Uչ���� ��
		class ScopedContext:public Context
		{
		public:
			ScopedContext(lua_State* l)
				:Context(l,false)
			{

			}
			ScopedContext(const LuaSpt& l)
				:Context(l.get(),false)
			{

			}
			ScopedContext(const Context& c)
				:Context(c.Get(),false)
			{

			}
			ScopedContext(const ScopedContext&)=delete;
			ScopedContext& operator=(const ScopedContext&)=delete;
		};

		//�� �����r ���� lua_pop ����
		class ScopedPop
		{
		protected:
			lua_State* _l;
			int _n;
		public:
			ScopedPop(lua_State* l,int n)
			{
				_l = l;
				_n = n;
			}
			ScopedPop(const Context& c,int n)
			{
				_l = c.Get();
				_n = n;
			}
			ScopedPop(const ScopedPop&)=delete;
			ScopedPop& operator=(const ScopedPop&)=delete;
			~ScopedPop()
			{
				if(_n && _l)
				{
					lua_pop(_l,_n);
				}
			}

			ScopedPop& operator+=(int n)
			{
				_n += n;
				return *this;
			}
			void operator++()
			{
				++_n;
			}
			ScopedPop& operator-=(int n)
			{
				_n += n;
				return *this;
			}
			void operator--()
			{
				--_n;
			}

		};
	};
};
#endif	//KING_LIB_HEADER_LUA_CONTEXT
