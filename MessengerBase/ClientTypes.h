#ifndef _MESSENGERBASE_CLIENT_TYPES_H_
#define _MESSENGERBASE_CLIENT_TYPES_H_

#include "messenger.h"

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

namespace MessageDirection
{
	enum Type
	{
		send,
		recv
	};
}

struct ExtensionType
{
	messenger::message_content_type::Type type;
	ImageType::Type image_type;
	VideoType::Type video_type;
};

struct MessageInfo
{
	messenger::Message message;
	MessageDirection::Type type;
};


#endif // _MESSENGERBASE_CLIENT_TYPES_H_
