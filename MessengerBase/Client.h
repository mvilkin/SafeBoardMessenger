#ifndef _MESSENGERBASE_CLIENT_H_
#define _MESSENGERBASE_CLIENT_H_

#include "messenger.h"
#include <future>
#include <unordered_map>

class Client : public messenger::ILoginCallback, messenger::IMessagesObserver, messenger::IRequestUsersCallback
{
public:
	Client();
	~Client();

	int EnterMessenger(const std::string& login, const std::string& password, const std::string& server);
	void ExitMessenger();
	void SendMessage(std::string user, std::string msg);
	messenger::UserList GetActiveUsers(bool update);
	void ReadNewMessages(std::string fromUserId);
	std::string MessagesToText(std::string fromUserId);

	void OnOperationResult(messenger::operation_result::Type result) override;
	void OnOperationResult(messenger::operation_result::Type result, const messenger::UserList& users) override;
	void OnMessageStatusChanged(const messenger::MessageId& msgId, messenger::message_status::Type status) override;
	void OnMessageReceived(const messenger::UserId& senderId, const messenger::Message& msg) override;

private:
	std::shared_ptr<messenger::IMessenger> m_messenger;
	std::mutex m_mutex;
	std::mutex m_mutex_msg;
	std::condition_variable m_cv;
	std::condition_variable m_cv_msg;
	std::condition_variable m_cv_usr;
	bool m_ready;
	bool m_status_changed;
	messenger::operation_result::Type m_enter_res;
	messenger::UserList m_userList;
	std::unordered_map<messenger::MessageId, messenger::message_status::Type> m_map_msg_statuses;
	std::unordered_map<messenger::UserId, std::vector<messenger::Message> > m_map_chat;
	std::unordered_map<messenger::UserId, std::vector<messenger::Message> > m_map_new_msg;
};

#endif // _MESSENGERBASE_CLIENT_H_