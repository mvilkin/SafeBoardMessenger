#ifndef _MESSENGERBASE_CLIENT_H_
#define _MESSENGERBASE_CLIENT_H_

#include "messenger.h"
#include <future>

class Client : public messenger::ILoginCallback, messenger::IMessagesObserver, messenger::IRequestUsersCallback
{
public:
	Client(const std::string& id);
	~Client();
	void SendMessage(std::string user, std::string msg);
	std::string ReceiveMessage();
	messenger::UserList GetActiveUsers(bool update);

	void OnOperationResult(messenger::operation_result::Type result) override;
	void OnOperationResult(messenger::operation_result::Type result, const messenger::UserList& users) override;
	void OnMessageStatusChanged(const messenger::MessageId& msgId, messenger::message_status::Type status) override;
	void OnMessageReceived(const messenger::UserId& senderId, const messenger::Message& msg) override;

private:
	std::shared_ptr<messenger::IMessenger> m_messenger;
	std::mutex m_mutex;
	std::condition_variable m_cv;
	bool m_ready;
	std::string m_receivedMsg;
	std::promise<messenger::UserList> m_userList;
};

#endif // _MESSENGERBASE_CLIENT_H_