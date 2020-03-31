/*
Copyright(c) 2016-2020 Panos Karabelas

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

//= INCLUDES ==================
#include <vector>
#include <string>
#include "../Core/ISubsystem.h"
//=============================

//= FORWARD DECLARATIONS =
class asIScriptObject;
class asIScriptFunction;
class asIScriptEngine;
class asIScriptContext;
class asIScriptModule;
class CScriptBuilder;
struct asSFuncPtr;
struct asSMessageInfo;
//========================

namespace Spartan
{
	class Module;

	class Scripting : public ISubsystem
	{
	public:
		Scripting(Context* context);
		~Scripting();

        //= Subsystem =============
        bool Initialize() override;
        //=========================

		void Clear();
		asIScriptEngine* GetAsIScriptEngine() const;

		// Contexts
		asIScriptContext* RequestContext();
		void ReturnContext(asIScriptContext* ctx);

		// Calls
		bool ExecuteCall(asIScriptFunction* scriptFunc, asIScriptObject* obj, float delta_time = -1.0f);

		// Modules
		void DiscardModule(const std::string& moduleName) const;

	private:
        asIScriptEngine* m_scriptEngine = nullptr;
		std::vector<asIScriptContext*> m_contexts;

		void LogExceptionInfo(asIScriptContext* ctx) const;
		void message_callback(const asSMessageInfo& msg) const;
	};
}
