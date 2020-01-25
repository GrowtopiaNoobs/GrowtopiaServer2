# PacketsEncoder

This is the only one header (and subheaders), which should call `Peer->sendPacket()`. If there is need for `SomePacket->send(OtherClassThanPeer x)`, then please implement `Peer operator=(OtherClassThanPeer rhs)` for your class.