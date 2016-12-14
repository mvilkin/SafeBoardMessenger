#ifndef _MESSENGERBASE_LOCALHISTORY_H_
#define _MESSENGERBASE_LOCALHISTORY_H_

#include <fstream>
#include <unordered_map>
#include "messenger.h"

void SaveMessageStatuses(std::ofstream& file, const std::unordered_map<messenger::MessageId, messenger::message_status::Type>& statuses);
void SaveCurrentChats(std::ofstream& file, const std::unordered_map<messenger::UserId, std::vector<messenger::Message> >& chat);
void SaveNewMessages(std::ofstream& file, const std::unordered_map<messenger::UserId, std::vector<messenger::Message> >& new_messages);

void LoadMessageStatuses(std::ifstream& file, std::unordered_map<messenger::MessageId, messenger::message_status::Type>& statuses);
void LoadCurrentChats(std::ifstream& file, std::unordered_map<messenger::UserId, std::vector<messenger::Message> >& chat);
void LoadNewMessages(std::ifstream& file, std::unordered_map<messenger::UserId, std::vector<messenger::Message> >& new_messages);

#endif // _MESSENGERBASE_LOCALHISTORY_H_
