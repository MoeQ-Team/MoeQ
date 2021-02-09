#pragma once
#include <stdint.h>

typedef unsigned char byte;
typedef unsigned int uint; 
typedef byte* LPBYTE;

namespace Message
{
	enum class MsgType
	{
		text = 1,
		classcal_face = 2,
		expression = 6,
		picture = 8,
		xml = 12,
		red_packet = 24,
		reply = 45,
		json = 51,
	};

	struct Data
	{
		uint Length = NULL;
		byte* Contain = nullptr;
		char8_t* URL = nullptr;
		char* Location = nullptr;
	};

	struct Msg
	{
		MsgType MsgType;
		Msg* NextPoint = nullptr;
		void* Message = nullptr;
	};

	struct text
	{
		char8_t* text;
		uint AtQQ; //if text == nullptr,this is a at,0=AtAll
	};

	struct classcal_face
	{
		uint id = NULL;
	};

	struct expression
	{
		uint id = NULL;
		byte* MD5 = nullptr;
	};

	struct picture
	{
		uint Width = NULL;
		uint Height = NULL;
		byte* MD5 = nullptr;
		Data Data;
	};

	struct voice
	{
		byte* MD5 = nullptr;
		Data Data;
	};

	struct xml
	{
		char8_t* text = nullptr;
	};

	struct json
	{
		char8_t* text = nullptr;
	};

	struct reply
	{
		uint MsgId;
		uint QQ;
		uint Time;
		Message::Msg* Msg = nullptr;
	};
}

namespace Target
{
	enum class TargetType
	{
		_private = 0,
		group = 1,
		discuss = 2,
	};

	struct Target
	{
		TargetType TargetType;
		void* Sender;
	};

	struct _private
	{
		uint FromQQ;
		// 0 FromFriend 1 From Online State 2 From Group 3 From Discuss
		// 0 ���Ժ��� 1 ��������״̬ 2 ����Ⱥ 3����������
		uint FromType;
	};

	struct group
	{
		uint FromGroup;
		uint FromQQ;
	};

	struct discuss
	{
		uint FromDiscuss;
		uint FromQQ;
	};
}

namespace Event
{
	enum class ReturnType
	{
		ignore, //Ignore ����
		block, //Block ����
	};

	namespace LifeCycleEvent
	{
		enum class LifeCycleEventType {
			StartUp,
			ShutDown,
			PluginEnabled,
			PluginDisabled
		};
	}

	namespace NoticeEvent
	{
		enum class NoticeEventType {
			group_fileupload, //Group file upload Ⱥ�ļ��ϴ�
			group_adminchange, //Group administrator changes Ⱥ����Ա�䶯
			group_memberchange, //The change in the number of group members Ⱥ��Ա�����䶯
			group_mute, //Group ban Ⱥ����
			friend_added, //Friend added ���������
		};


		struct NoticeEvent
		{
			Event::NoticeEvent::NoticeEventType NoticeEventType;
			void* Information;
		};

		struct FileInfo
		{
			char* Name;
			char* ID;
			unsigned long long size;
		};

		struct group_fileupload
		{
			uint FromGroup;
			uint FromQQ;
			FileInfo File;
		};
		struct group_adminchange
		{
			uint FromGroup;
			uint FromQQ;
			uint Type; // 0 Cancelled administrator ��ȡ������Ա 1 Set up administrator �����ù���Ա
		};
		struct group_memberchange
		{
			uint Type; // 0 Increase(Don't include Invited) ����(������������) 1 Invited ������ 2 Decrease(Don't include kicked and disband) ����(���������ߺͽ�ɢ) 3 Kicked ���� 4 Disband ��ɢ
			uint FromGroup;
			uint FromQQ;
			uint OperateQQ; //TypeΪ1,3����
		};
		struct group_mute
		{
			uint FromGroup;
			uint FromQQ;
			uint OperateQQ;
			uint Type; // 0 Ban ������ 1 Free ����� 
		};
		struct friend_added
		{
			uint FromQQ;
		};
	}

	namespace RequestEvent
	{
		enum class RequestEventType {
			add_friend,
			add_group
		};

		enum class ReturnType
		{
			agree, //Agree ͬ��
			disagree, //Disagree ��ͬ��
			ignore, //Ignore ����
		};

		struct RequestEvent
		{
			Event::RequestEvent::RequestEventType RequestEventType;
			void* Information;
		};

		struct add_friend
		{
			uint FromQQ;
			char8_t* msg;
		};
		struct add_group
		{
			uint FromGroup;
			uint FromQQ;
			char8_t* msg;
			uint Type; // 0 Others apply to join the group ����������Ⱥ 1 Myself was invited to join the group �Լ�������Ⱥ
		};
	}

	struct GroupMsg
	{
		uint FromQQ;
		uint FromGroup;
		char8_t* FromQQName;
		char8_t* FromGroupName;

		uint SendTime;
		uint MsgType;
		uint MsgID;
		uint MsgRand;

		Message::Msg* Msg = nullptr;
	};

	struct PrivateMsg
	{
		uint FromQQ;
		char8_t* FromQQName;

		uint SendTime;
		uint MsgType;
		uint MsgID;
		uint MsgRand;


		Message::Msg* Msg = nullptr;//u8
	};
}

namespace Log
{
	enum class LogType
	{
		__DEBUG = 0,
		INFORMATION = 1,
		NOTICE = 2,
		WARNING = 3,
		_ERROR = 4
	};

	enum class MsgType
	{
		SERVER = 0,
		PROGRAM = 1,
		PRIVATE = 2,
		_GROUP = 3,
		OTHER = 4
	};

	struct Log {
		LogType LogType;
		MsgType MsgType;
		const wchar_t* Type;
		const wchar_t* Msg;
	};
}