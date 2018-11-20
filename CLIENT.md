# Excom Client Session Persistence

Excom is, in its nature, designed to be decentralized.  No one server,
service, or IP address should have precedence over any other - and this
makes authentication, or persistance of authentication, difficult.  How
do you determine that a given client is the same, or different from,
another client? Across different servers?  How do you verify that a
given client is, in fact, who they say they are?

The idea is to simplify it down to a "client identity."  A client
identity is a public/private key pair, where the private key is
encrypted with a password.  When a user logs into a given client, they
decrypt the private key with the password.  Then, when a user first
connects to a server, they provide their public key.  Then, they prove
that they have the private key corresponding to that public key.  This
public key is then used as the identifier for the client for that
server - whenever it is used, the server knows which client is which.

This, however, requires that the client knows its private key.  This is
problematic if, and when, the client moves devices.  The new device
will not have the private key, and so the client cannot authenticate
with the server, unless they take a new identity! This is obviously not
desirable!  There are a few solutions to this:

1. Encrypt the private key with a password, and send the encrypted
   private key to the server.  Then, when on a new device, the client
   can attempt a login with the password; if it succeeds, the server can
   send the encrypted private key to the new device, which can then be
   used to log in.  This requires that the client trusts the server -
   if the server so decided, it can perform dictionary attacks on the
   private key; or, if the server was breached, the encrypted private
   keys would be released; or, when the password is provided to the
   server, the server can keep an unencrypted copy of the key.  (The
   other option, to avoid sending the password to the server, is to have
   the server send the encrypted private key to any client who asks;
   but this is just as bad as having the encrypted private keys being
   released).
2. Derive the private key solely from the password.  The private/public
   key pair would _only_ be derived from the password, meaning that the
   password is, in effect, the private key.  However, this has pitfalls;
   namely, the public key is now susceptible to an offline dictionary
   attack - which is bad, since the password was used in its
   construction.  However, this is somewhat mitigated by the fact that
   only servers the client has joined will have the public key.

   Performing the derivation with only a password is also difficult;
   most cryptographic implementations are not ready, nor expecting, this
   sort of derivation.  In effect, we would have to hijack the random
   number generator to make it deterministic, using the password (or
   a direct derivation) as a seed.
