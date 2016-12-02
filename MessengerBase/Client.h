#ifndef _MESSENGERBASE_CLIENT_H_
#define _MESSENGERBASE_CLIENT_H_

#include "messenger.h"

class Client : public messenger::ILoginCallback, messenger::IMessagesObserver, messenger::IRequestUsersCallback
{
public:
	Client();
	~Client();
	void SendMessage(std::string user, std::string msg);
	std::string ReceiveMessage();
	messenger::UserList GetActiveUsers();

	void OnOperationResult(messenger::operation_result::Type result) override;
	void OnMessageStatusChanged(const messenger::MessageId& msgId, messenger::message_status::Type status) override;
	void OnMessageReceived(const messenger::UserId& senderId, const messenger::Message& msg) override;
	void OnOperationResult(messenger::operation_result::Type result, const messenger::UserList& users) override;
};

#endif // _MESSENGERBASE_CLIENT_H_