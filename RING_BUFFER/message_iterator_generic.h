#ifndef MESSAGE_ITERATOR_GENERIC_H
#define MESSAGE_ITERATOR_GENERIC_H
#include <cstdint>
#include <cstring>
#include <QDebug>
#include "message_struct_generic.h"

//template<HEADER_GENERIC H> class MessageIteratorGenericBase;
enum class IteratorMode {Chunked = 0, ChunkedContinous = 1, Continous = 2};

template<typename H>
class MessageIteratorGenericBase
{
public:
    MessageIteratorGenericBase() { PtrBufferBegin = 0; PtrBufferEnd = 0; PtrMessageBegin = 0; PtrDataEnd = 0; }
    MessageIteratorGenericBase(uint8_t* STORAGE, uint16_t ChunkSize, uint16_t NumberChunks);
	struct MessagesRange
	{
		uint8_t* EndMessageBuffer;
		uint8_t* LastMessage;
	};

public:
	virtual void LoadData(uint8_t* DataSourceBuffer, uint16_t BytesToLoad) = 0;
    MessageIteratorGenericBase<H> operator++(int) { SwitchToNext(); return* this; }

	MessageGeneric<void*,H>& operator*() { return *reinterpret_cast<MessageGeneric<void*,H>* >(PtrMessageBegin); }
	MessageGeneric<void*,H>* GetMessagePtr() { return reinterpret_cast<MessageGeneric<void*,H>* >(PtrMessageBegin); }

	bool IsHeaderValid()  { return GetHeader().isValid(); }
	void ResetIterator()  { PtrMessageBegin = PtrBufferBegin; PtrDataEnd = PtrBufferBegin; }
	 int DataAvailable()   { return PtrDataEnd - PtrMessageBegin; }

	 int MessageSize();
	 int MessageSize(uint8_t* PtrMessage);


	  H* GetHeaderPtr()  { return  reinterpret_cast<H*>(PtrMessageBegin); }
	  H& GetHeader()     { return *reinterpret_cast<H*>(PtrMessageBegin); }
	  H& GetHeader(uint8_t* PtrHeader) { return *reinterpret_cast<H*>(PtrHeader); }

	 int StepsTo(const MessageIteratorGenericBase<H>& It) { return It.MessageNumber - MessageNumber;}; 
	 int BytesTo(const MessageIteratorGenericBase<H>& It) { return It.PtrMessageBegin - PtrMessageBegin;}; 

	void operator= (const MessageIteratorGenericBase& Message);
	bool operator==(MessageIteratorGenericBase<H>& Message) { return PtrMessageBegin == Message.PtrMessageBegin; }
	bool operator!=(MessageIteratorGenericBase<H>& Message) { return !(Message == *this); }

	MessagesRange RangeLimits;
	uint32_t MessageNumber = 0;

    void PrintIterator();
	protected:
	uint8_t* PtrMessageBegin;
	uint8_t* PtrDataEnd = 0;

	uint8_t* PtrBufferBegin;
	uint8_t* PtrBufferEnd;

	uint16_t  BufferChunkSize = 100;
	uint16_t  MessageDataSize = 25;
	
	void SwitchToNext();
};

template<typename H>
MessageIteratorGenericBase<H>::MessageIteratorGenericBase(uint8_t* STORAGE, uint16_t ChunkSize, uint16_t NumberChunks)
{
	BufferChunkSize = ChunkSize;
	PtrBufferBegin = STORAGE; PtrBufferEnd = STORAGE + NumberChunks*ChunkSize;
	PtrMessageBegin = PtrBufferBegin; PtrDataEnd = PtrMessageBegin;

	RangeLimits.EndMessageBuffer = PtrBufferEnd;
	RangeLimits.LastMessage = PtrMessageBegin; 
}

template<typename H>
void MessageIteratorGenericBase<H>::operator=(const MessageIteratorGenericBase<H>& Message)
{
	PtrMessageBegin = Message.PtrMessageBegin; 
	     PtrDataEnd = Message.PtrDataEnd;
	 PtrBufferBegin = Message.PtrBufferBegin;
 	   PtrBufferEnd = Message.PtrBufferEnd;
	   BufferChunkSize = Message.BufferChunkSize;
	   RangeLimits.EndMessageBuffer = Message.RangeLimits.EndMessageBuffer;
	   RangeLimits.LastMessage      = Message.RangeLimits.LastMessage;
}

template<typename H> int MessageIteratorGenericBase<H>::MessageSize(uint8_t* PtrMessage)
{
	auto& Header = GetHeader(PtrMessage); 
	  if(!Header.isValid()) return 0; 
						    return Header.getMessageSize();
}
template<typename H> int MessageIteratorGenericBase<H>::MessageSize()      
{ 
	auto& Header = GetHeader(); 
	  if(!Header.isValid()) return 0; 
						    return Header.getMessageSize();
};

template<typename H>
void MessageIteratorGenericBase<H>::PrintIterator()
{
qDebug() << "HEADER: " << IsHeaderValid() << "BEGIN: " << PtrMessageBegin << "BEGIN REL: " << PtrMessageBegin - PtrBufferBegin;
}

template<typename H>
void MessageIteratorGenericBase<H>::SwitchToNext()
{
 if(this->PtrMessageBegin == this->RangeLimits.LastMessage) return;
    this->PtrMessageBegin += this->BufferChunkSize; 

 if(this->PtrMessageBegin == this->RangeLimits.EndMessageBuffer) 
    this->PtrMessageBegin =  this->PtrBufferBegin; 
}


//====================================================================================================
template<HEADER_STANDART H>
class MessageIteratorGenericBase<H>
{
public:
    MessageIteratorGenericBase() { PtrBufferBegin = 0; PtrBufferEnd = 0; PtrMessageBegin = 0; PtrDataEnd = 0; } //qDebug() << "[ CREATE ITERATOR TO STANDART DATA ]"; }
    MessageIteratorGenericBase(uint8_t* STORAGE, uint16_t ChunkSize, uint16_t NumberChunks);
	struct MessagesRange
	{
		uint8_t* EndMessageBuffer;
		uint8_t* LastMessage;
	};

public:
	virtual void LoadData(uint8_t* DataSourceBuffer, uint16_t BytesToLoad) = 0;

	     MessageGeneric<void*,H>* GetMessagePtr() { return reinterpret_cast<MessageGeneric<void*,H>* >(PtrMessageBegin); }
	     MessageGeneric<void*,H>& operator*()     { return *reinterpret_cast<MessageGeneric<void*,H>* >(PtrMessageBegin); }

	void ResetIterator()  { PtrMessageBegin = PtrBufferBegin; PtrDataEnd = PtrBufferBegin; }

    bool IsHeaderValid()  { return GetHeader().isValid(); }
	int  DataAvailable()  { return PtrDataEnd - PtrMessageBegin; }

	int MessageSize();
	int MessageSize(uint8_t* PtrMessage);

	 H* GetHeaderPtr()  { return  reinterpret_cast<H*>(PtrMessageBegin); }
	 H& GetHeader()     { return *reinterpret_cast<H*>(PtrMessageBegin); }
	 H& GetHeader(uint8_t* PtrHeader) { return *reinterpret_cast<H*>(PtrHeader); }

	int StepsTo(const MessageIteratorGenericBase<H>& It) { return It.MessageNumber - MessageNumber;}; 
	int BytesTo(const MessageIteratorGenericBase<H>& It) { return It.PtrMessageBegin - PtrMessageBegin;}; 

	void operator=(const MessageIteratorGenericBase& Message);
    bool operator!=(MessageIteratorGenericBase<H>& Message) { return !(Message == *this); }
    bool operator==(MessageIteratorGenericBase<H>& Message) { return PtrMessageBegin == Message.PtrMessageBegin; }

	MessagesRange RangeLimits;
	uint32_t MessageNumber = 0;

    void PrintIterator();
	protected:
	uint8_t* PtrMessageBegin;
	uint8_t* PtrDataEnd = 0;

	uint8_t* PtrBufferBegin;
	uint8_t* PtrBufferEnd;

	uint8_t  BufferChunkSize = 100;
	uint8_t  MessageDataSize = 25;
	
	void SwitchToNext();
};

template<HEADER_STANDART H>
MessageIteratorGenericBase<H>::MessageIteratorGenericBase(uint8_t* STORAGE, uint16_t ChunkSize, uint16_t NumberChunks)
{
	BufferChunkSize = ChunkSize;
	PtrBufferBegin = STORAGE; PtrBufferEnd = STORAGE + NumberChunks*ChunkSize;
	PtrMessageBegin = PtrBufferBegin; PtrDataEnd = PtrMessageBegin;

	RangeLimits.EndMessageBuffer = PtrBufferEnd;
	RangeLimits.LastMessage = PtrMessageBegin; 
}

template<HEADER_STANDART H>
void MessageIteratorGenericBase<H>::operator=(const MessageIteratorGenericBase<H>& Message)
{
	PtrMessageBegin = Message.PtrMessageBegin; 
	     PtrDataEnd = Message.PtrDataEnd;
	 PtrBufferBegin = Message.PtrBufferBegin;
 	   PtrBufferEnd = Message.PtrBufferEnd;
	   BufferChunkSize = Message.BufferChunkSize;
	   RangeLimits.EndMessageBuffer = Message.RangeLimits.EndMessageBuffer;
	   RangeLimits.LastMessage      = Message.RangeLimits.LastMessage;
}

template<HEADER_STANDART H> int MessageIteratorGenericBase<H>::MessageSize(uint8_t* PtrMessage)
{
	auto& Header = GetHeader(PtrMessage); 
	if(!Header.isValid()) return 0; 
						  return Header.DATA_SIZE + sizeof(H);
}
template<HEADER_STANDART H> int MessageIteratorGenericBase<H>::MessageSize()      
{ 
	auto& Header = GetHeader(); 
	if(!Header.isValid()) return 0; 
						  return Header.DATA_SIZE + sizeof(H);
};


template<HEADER_STANDART H>
void MessageIteratorGenericBase<H>::PrintIterator()
{
qDebug() << "HEADER: " << IsHeaderValid() << "BEGIN: " << PtrMessageBegin << "BEGIN REL: " << PtrMessageBegin - PtrBufferBegin;
}

template<HEADER_STANDART H>
void MessageIteratorGenericBase<H>::SwitchToNext()
{
 if(this->PtrMessageBegin == this->RangeLimits.LastMessage) return;
    this->PtrMessageBegin += this->BufferChunkSize; 

 if(this->PtrMessageBegin == this->RangeLimits.EndMessageBuffer) 
    this->PtrMessageBegin =  this->PtrBufferBegin; 
}

//=============================================================

template<typename H, IteratorMode Mode=IteratorMode::Chunked>
class MessageIteratorGeneric : public MessageIteratorGenericBase<H> 
{
public:
    MessageIteratorGeneric(): MessageIteratorGenericBase<H>()       {};
    MessageIteratorGeneric(uint8_t* STORAGE, uint16_t ChunkSize, uint16_t NumberChunks) :
	MessageIteratorGenericBase<H>(STORAGE, ChunkSize, NumberChunks) {};

	uint16_t BytesAvailable = 0;
public:

    MessageIteratorGeneric<H,IteratorMode::Chunked> operator++(int) { this->SwitchToNext(); return* this; };
	void LoadData(uint8_t* DataSourceBuffer, uint16_t BytesToLoad) override
	{
		         //if(!this->GetHeader(DataSourceBuffer).isValid()) return;

		std::memcpy(this->PtrDataEnd,DataSourceBuffer, BytesToLoad); 
					this->PtrDataEnd += BytesToLoad; 
        	          BytesAvailable += BytesToLoad;

		while(BytesAvailable >= this->BufferChunkSize)
		{
           this->PtrMessageBegin += this->BufferChunkSize; this->MessageNumber++;
		          BytesAvailable -= this->BufferChunkSize;
		   this->RangeLimits.LastMessage = this->PtrMessageBegin;
		}

		if(this->PtrBufferEnd - this->PtrDataEnd < 2*this->BufferChunkSize) 
		{
		   //qDebug() << "BYTES AVAILABLE AT RESET: " << BytesAvailable;
		   this->RangeLimits.EndMessageBuffer = this->PtrMessageBegin;

		   if(BytesAvailable > 0) std::memcpy(this->PtrBufferBegin,this->PtrMessageBegin, BytesAvailable); 
		   this->PtrMessageBegin =  this->PtrBufferBegin; 
		   this->PtrDataEnd      =  this->PtrBufferBegin + BytesAvailable; 

		}

	}
};

template<typename H>
class MessageIteratorGeneric<H,IteratorMode::ChunkedContinous> : public MessageIteratorGenericBase<H> 
{
public:
    MessageIteratorGeneric(): MessageIteratorGenericBase<H>() {};
    MessageIteratorGeneric(uint8_t* STORAGE, uint16_t ChunkSize, uint16_t NumberChunks): 
	MessageIteratorGenericBase<H>(STORAGE, ChunkSize, NumberChunks) 
	{  
       qDebug() << "[ CREATE ITERATOR CHUNKED CONTINOUS ] MARKER: " << Qt::hex << HeaderMarker;
	};

	      uint8_t* PtrNextHeader = nullptr;
	      uint8_t HeaderSearchDepth = MessageIteratorGenericBase<H>::BufferChunkSize/4;
		  uint32_t MessageSize = 0;
		   uint8_t HeaderMarker = H::getHeaderMarker();

public:
    MessageIteratorGeneric<H,IteratorMode::ChunkedContinous> operator++(int) { this->SwitchToNext(); return* this; };
	bool FindHeader(uint8_t* DataSourceEnd   , uint16_t SearchDepth)
	{
		   PtrNextHeader = std::find(DataSourceEnd - SearchDepth, DataSourceEnd, HeaderMarker );
		                      if(PtrNextHeader == DataSourceEnd ) return false;
        if(!reinterpret_cast<H*>(PtrNextHeader)->isValid()) return false;
										                return true;
	}

	void LoadData  (uint8_t* DataSourceBuffer, uint16_t NumberBytesToLoad) override
	{
	                                                          if(NumberBytesToLoad <= 0) return;
	 //qDebug() << "LOAD DATA CHUNKED CONTINOUS" << NumberBytesToLoad ;

		std::memcpy(this->PtrDataEnd, DataSourceBuffer, NumberBytesToLoad); 
					this->PtrDataEnd += this->BufferChunkSize; 
					                                this->PtrMessageBegin = this->PtrDataEnd;
		            this->RangeLimits.LastMessage = this->PtrMessageBegin; 
					this->MessageNumber++;

		if(this->PtrDataEnd == this->RangeLimits.EndMessageBuffer) //RESET TO BEGIN
		{
		   this->PtrMessageBegin  = this->PtrBufferBegin; 
		   this->PtrDataEnd  = this->PtrBufferBegin; 
		}
	 //qDebug() << "END LOAD DATA" << this->PtrBufferEnd - this->PtrDataEnd ;

//	 this->PtrMessageBegin = this->PtrDataEnd;
//	 	                     this->PtrDataEnd += this->BufferChunkSize; 
//
//	 	 if(FindHeader(this->PtrDataEnd ,HeaderSearchDepth)) 
//	 	 {
//                                      MessageSize = PtrNextHeader - this->PtrMessageBegin; 
//                                                   NumberBytesToLoad -= MessageSize;
//          LoadData(DataSourceBuffer + MessageSize, NumberBytesToLoad);
//	 	 }

	}

};
//====================================================================================================

template<typename H>
class MessageIteratorGeneric<H,IteratorMode::Continous> : public MessageIteratorGenericBase<H> 
{
public:
    MessageIteratorGeneric(): MessageIteratorGenericBase<H>() {};
    MessageIteratorGeneric(uint8_t* STORAGE, uint16_t ChunkSize, uint16_t NumberChunks) :
	MessageIteratorGenericBase<H>(STORAGE, ChunkSize, NumberChunks) { };

protected:

    bool IsMemoryAtEnd()   { return (this->PtrBufferEnd - this->PtrMessageBegin) < 2*this->BufferChunkSize; }
     int MemoryAvailable() { return (this->PtrBufferEnd - this->PtrMessageBegin) - 2*this->BufferChunkSize; }
     int DataAvailable()   { return std::abs(this->PtrDataEnd - this->PtrMessageBegin); }
    void MoveDataToBegin();

	   H* HeaderCurrent     = nullptr;
	 uint32_t SpaceAvailable = 0;
	 uint32_t DataRemain  = 0;

public:
    MessageIteratorGeneric<H,IteratorMode::Continous> operator++(int); 

	void LoadData(uint8_t* DataSourceBuffer, uint16_t BytesToLoad) override
	{

	std::memcpy(this->PtrDataEnd,DataSourceBuffer,BytesToLoad); this->PtrDataEnd += BytesToLoad;

	    //===================================================================
        //PARSE MESSAGES
			  HeaderCurrent = this->GetHeaderPtr();                  BytesToLoad  = this->PtrDataEnd - this->PtrMessageBegin;
		while(HeaderCurrent->isValid() && HeaderCurrent->getMessageSize() <= BytesToLoad)
		{
			this->MessageNumber++;          this->PtrMessageBegin += HeaderCurrent->getMessageSize(); 
			this->RangeLimits.LastMessage = this->PtrMessageBegin;   

			  HeaderCurrent = this->GetHeaderPtr();                  BytesToLoad  = this->PtrDataEnd - this->PtrMessageBegin;

		}
	    //===================================================================

	    //===================================================================
		//MOVE AVAILABLE DATA TO BEGIN IF BUFFER AT END
		   SpaceAvailable = this->PtrBufferEnd - this->PtrDataEnd;
		if(SpaceAvailable < 2*this->BufferChunkSize) 
		{
			this->RangeLimits.EndMessageBuffer = this->PtrMessageBegin;
			//qDebug() << "MOVE DATA TO END: REMAIN: " << BytesToLoad;
			std::memcpy(this->PtrBufferBegin,
						this->PtrMessageBegin,BytesToLoad); 
						this->PtrMessageBegin = this->PtrBufferBegin;
						      this->PtrDataEnd = this->PtrBufferBegin  + BytesToLoad;
		} 
	    //===================================================================
	}


//	void LoadData(uint8_t* DataSourceBuffer, uint16_t BytesToLoad) override
//	{
//	std::memcpy(this->PtrDataEnd,DataSourceBuffer,BytesToLoad); this->PtrDataEnd += BytesToLoad;
//
//		while(DataAvailable() >= this->MessageSize() && this->IsHeaderValid())
//			{
//
//				this->PtrMessageBegin += this->MessageSize(); this->RangeLimits.LastMessage = this->PtrMessageBegin;
//				this->MessageNumber++;
//
//				if(IsMemoryAtEnd()) 
//				{
//					this->RangeLimits.EndMessageBuffer = this->PtrMessageBegin;
//					MoveDataToBegin();
//				} 
//
//			}
//	}
};


    template<typename H>
    MessageIteratorGeneric<H,IteratorMode::Continous> MessageIteratorGeneric<H, IteratorMode::Continous>::operator++(int) 
    {
     if(this->PtrMessageBegin == this->RangeLimits.LastMessage) return *this;
    
     auto& Header = this->GetHeader(); if(!Header.isValid()) { this->ResetIterator(); return *this;};
    
        this->PtrMessageBegin += this->MessageSize(); 
     if(this->PtrMessageBegin == this->RangeLimits.EndMessageBuffer) 
        this->PtrMessageBegin =  this->PtrBufferBegin; 
    
     return* this;
    }
    
    template<typename H>
    void MessageIteratorGeneric<H,IteratorMode::Continous>::MoveDataToBegin()
    {
    	                                               uint8_t DataRemain = this->DataAvailable();
    	            this->PtrDataEnd = this->PtrBufferBegin  + DataRemain;
    	std::memcpy(this->PtrBufferBegin,this->PtrMessageBegin,DataRemain); 
    	            this->PtrMessageBegin = this->PtrBufferBegin;
    }

//====================================================================================================

#endif 


			//qDebug() << "=================================" ;
			//qDebug() << "LOAD DATA: " << BytesToLoad ;
                //qDebug() << "APPEND MESSAGE SIZE: " << this->MessageSize();
                //qDebug() << "=================================" ;
//  CONTINOUS LOAD
//=============================================================== 

	//qDebug() << "====================================" ;
	//qDebug() << "[ LOAD DATA ]" ;
	//		qDebug() << "SPACE REMAIN:" << SpaceAvailable << "THRES:" << 2*this->BufferChunkSize;
	//		qDebug() << "MOVE DATA TO END: REMAIN: " << DataRemain;
	//qDebug() << "====================================" ;