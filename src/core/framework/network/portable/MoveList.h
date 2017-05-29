//
//  MoveList.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__MoveList__
#define __noctisgames__MoveList__

#include "InputState.h"

#include "Move.h"

#include <deque>

class MoveList
{
public:
    MoveList():
    mLastMoveTimestamp(-1.f)
    {}
    
    const	Move&	AddMove(const InputState& inInputState, float inTimestamp);
    bool	AddMoveIfNew(const Move& inMove);
    
    void	RemovedProcessedMoves(float inLastMoveProcessedOnServerTimestamp);
    
    float			GetLastMoveTimestamp()	const	{ return mLastMoveTimestamp; }
    
    const Move&		GetLatestMove()			const	{ return mMoves.back(); }
    
    void			Clear()							{ mMoves.clear(); }
    bool			HasMoves()				const	{ return !mMoves.empty(); }
    int				GetMoveCount()			const	{ return static_cast<int>(mMoves.size()); }
    
    std::deque< Move >::const_iterator	begin()					const	{ return mMoves.begin(); }
    std::deque< Move >::const_iterator	end()					const	{ return mMoves.end(); }
    
private:
    float			mLastMoveTimestamp;
    std::deque< Move >	mMoves;
};

#endif /* defined(__noctisgames__MoveList__) */