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

//= INCLUDES =====================
#include "Script.h"			
#include "../../Core/Context.h"
#include "../../Core/FileSystem.h"
#include "../../IO/FileStream.h"
#include "../Entity.h"
//================================

//= NAMESPACES =====
using namespace std;
//==================

namespace Spartan
{
	Script::Script(Context* context, Entity* entity, uint32_t id /*= 0*/) : IComponent(context, entity, id)
	{

	}

    //= ICOMPONENT ==================================================================
	void Script::OnStart()
	{
		if (!m_scriptInstance)
			return;

		if (!m_scriptInstance->IsInstantiated())
			return;

		m_scriptInstance->ExecuteStart();
	}

	void Script::OnTick(float delta_time)
	{
		if (!m_scriptInstance)
			return;

		if (!m_scriptInstance->IsInstantiated())
			return;

		m_scriptInstance->ExecuteUpdate(delta_time);
	}

	void Script::Serialize(FileStream* stream)
	{
		stream->Write(m_scriptInstance ? m_scriptInstance->GetScriptPath() : "");
	}

	void Script::Deserialize(FileStream* stream)
	{
		string script_path;
		stream->Read(&script_path);

		if (!script_path.empty())
		{
			SetScript(script_path);
		}
	}
	//====================================================================================

	bool Script::SetScript(const string& filePath)
	{
		// Instantiate the script
		m_scriptInstance = make_shared<ScriptInstance>();
		m_scriptInstance->Instantiate(filePath, GetEntity()->GetPtrShared(), GetContext()->GetSubsystem<Scripting>());

		// Check if the script has been instantiated successfully.
		if (!m_scriptInstance->IsInstantiated())
			return false;

		m_scriptInstance->ExecuteStart();
		return true;
	}

	string Script::GetScriptPath() const
    {
		return m_scriptInstance ? m_scriptInstance->GetScriptPath() : "";
	}

	string Script::GetName()
	{
		return m_scriptInstance ? FileSystem::GetFileNameNoExtensionFromFilePath(GetScriptPath()) : "";
	}
}
