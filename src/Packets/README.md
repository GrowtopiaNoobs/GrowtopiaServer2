# Packets

This set of header implements functions, structures and classes for decoding, encoding and encoding of packets.

Use of different files is described here:
- `PacketsCommon` contains files, which should be included only by `PacketsEncoder` and `PacketsDeoder`. It is used for stuff which is needed for both of them.
- `PacketsDecoder` contains files for decoding packet.
- `PacketsEncoder` contains files for encoding packet and sending it to Peer.