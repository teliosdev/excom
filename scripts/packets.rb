ok 0x01 do
end

reject 0x02 do
  number :uint8_t, :reason
end

protocol_version 0x03 do
  string :version
  number :uint8_t, :major
  number :uint8_t, :minor
  number :uint8_t, :patch
end
