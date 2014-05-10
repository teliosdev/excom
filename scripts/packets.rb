ok 0x01, :both do
end

reject 0x02, :both do
  number :uint8_t, :reason
end

error 0x03, :both do
  number :uint8_t, :reason
end

protocol_version 0x04, :both do
  string :version
  number :uint8_t, :major
  number :uint8_t, :minor
  number :uint8_t, :patch
end

ping 0x05, :both do
end

public_key 0x06, :both do
  string :key
end

command 0x10, :client do
  string :command
end

command_response 0x11, :server do
  string :response
end
