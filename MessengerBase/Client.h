#ifndef _MESSENGERBASE_CLIENT_H_
#define _MESSENGERBASE_CLIENT_H_

#include "messenger.h"
#include <future>
#include <unordered_map>

namespace ImageType
{
	enum Type
	{
		bmp,
		jpg,
		png,
		undef
	};
}

namespace VideoType
{
	enum Type
	{
		avi,
		mkv,
		undef
	};
}

struct ExtensionType
{
	messenger::message_content_type::Type type;
	ImageType::Type image_type;
	VideoType::Type video_type;
};

class Client : public messenger::ILoginCallback, messenger::IMessagesObserver, messenger::IRequestUsersCallback
{
public:
	Client();
	~Client();

	int EnterMessenger(std::string login, std::string password, std::string server, int port);
	void ExitMessenger();

	void SendNewMessage(messenger::UserId user_id, std::string message);
	void SendNewFile(messenger::UserId user_id, std::string path);
	void StartReceivingProcess();
	void StopReceivingProcess();
	bool ReadNewMessages(messenger::UserId user_id);
	std::string MessagesToText(messenger::UserId user_id);

	void StartUpdatingProcess();
	void StopUpdatingProcess();
	messenger::UserList GetActiveUsers();
	std::string GetActiveUsersString();
	bool CheckUserNewMessages(messenger::UserId user_id);

	void OnOperationResult(messenger::operation_result::Type result) override;
	void OnOperationResult(messenger::operation_result::Type result, const messenger::UserList& users) override;
	void OnMessageStatusChanged(const messenger::MessageId& message_id, messenger::message_status::Type status) override;
	void OnMessageReceived(const messenger::UserId& user_id, const messenger::Message& message) override;

private:
	static messenger::Data readFileBinary(std::string path);
	static std::string writeFileBinary(const messenger::Data& data, time_t time);
	static messenger::message_content_type::Type detectFileContentType(const messenger::Data& data);
	static std::string detectFileExtension(const messenger::Data& data);
	static ExtensionType detectFileExtensionType(const messenger::Data& data);

	std::shared_ptr<messenger::IMessenger> m_messenger;
	
	std::mutex m_mutex_init;
	std::condition_variable m_cv_init;
	bool m_is_inited;
	messenger::operation_result::Type m_init_result;

	std::mutex m_mutex_msg;
	std::condition_variable m_cv_msg;
	bool m_status_changed;
	bool m_recv_process;

	std::mutex m_mutex_usr;
	std::condition_variable m_cv_usr;
	bool m_update_process;
	messenger::UserList m_userList;

	std::unordered_map<messenger::MessageId, messenger::message_status::Type> m_map_msg_statuses;
	std::unordered_map<messenger::UserId, std::vector<messenger::Message> > m_map_chat;
	std::unordered_map<messenger::UserId, std::vector<messenger::Message> > m_map_new_msg;
};

#endif // _MESSENGERBASE_CLIENT_H_