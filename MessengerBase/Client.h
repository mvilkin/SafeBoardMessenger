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

	int EnterMessenger(std::wstring login, std::wstring password, std::wstring server);
	void ExitMessenger();
	void SendNewMessage(std::wstring user, std::wstring message);

	void StartReceivingProcess();
	void StopReceivingProcess();
	bool ReadNewMessages(std::wstring user);
	std::wstring MessagesToText(std::wstring user);

	void StartUpdatingProcess();
	void StopUpdatingProcess();
	messenger::UserList GetActiveUsers();
	std::wstring GetActiveUsersString();
	bool CheckUserNewMessages(messenger::UserId user);

	void OnOperationResult(messenger::operation_result::Type result) override;
	void OnOperationResult(messenger::operation_result::Type result, const messenger::UserList& users) override;
	void OnMessageStatusChanged(const messenger::MessageId& msgId, messenger::message_status::Type status) override;
	void OnMessageReceived(const messenger::UserId& senderId, const messenger::Message& msg) override;

private:
	std::wstring NormalToWide(std::string string_to_convert);
	std::string WideToNormal(std::wstring string_to_convert);

	std::shared_ptr<messenger::IMessenger> m_messenger;
	std::mutex m_mutex;
	std::mutex m_mutex_msg;
	std::condition_variable m_cv_init;
	std::condition_variable m_cv_msg;
	std::condition_variable m_cv_usr;
	bool m_inited;
	bool m_status_changed;
	bool m_recv_process;
	bool m_update_process;
	messenger::operation_result::Type m_enter_res;
	messenger::UserList m_userList;
	std::unordered_map<messenger::MessageId, messenger::message_status::Type> m_map_msg_statuses;
	std::unordered_map<messenger::UserId, std::vector<messenger::Message> > m_map_chat;
	std::unordered_map<messenger::UserId, std::vector<messenger::Message> > m_map_new_msg;
};

#endif // _MESSENGERBASE_CLIENT_H_