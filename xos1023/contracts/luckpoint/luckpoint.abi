{
  "version": "eosio::abi/1.0",
  "types": [{
      "new_type_name": "time",
      "type": "uint32"
    }
  ],
  "structs": [{
      "name": "global",
      "base": "",
      "fields": [{
          "name": "id",
          "type": "uint64"
        },{
          "name": "nextgameid",
          "type": "uint64"
        }
      ]
    },{
      "name": "game",
      "base": "",
      "fields": [{
          "name": "id",
          "type": "uint64"
        },{
          "name": "winner",
          "type": "uint32"
        },{
          "name": "player1",
          "type": "uint32"
        },{
          "name": "player2",
          "type": "uint32"
        },{
          "name": "createtime",
          "type": "time"
        },{
          "name": "owner",
          "type": "name"
        }
      ]
    },{
      "name": "creategame",
      "base": "",
      "fields": [{
          "name": "_owner",
          "type": "name"
        }
      ]
    },{
      "name": "opencard",
      "base": "",
      "fields": [{
          "name": "_gameid",
          "type": "uint64"
        },{
          "name": "_player",
          "type": "uint32"
        }
      ]
    },{
      "name": "sayhi",
      "base": "",
      "fields": [{
          "name": "receiver",
          "type": "name"
        }
      ]
    },{
      "name": "printrand",
      "base": "",
      "fields": [{
          "name": "_gameid",
          "type": "uint64"
        }
      ]
    }
  ],
  "actions": [{
      "name": "creategame",
      "type": "creategame",
      "ricardian_contract": ""
    },{
      "name": "opencard",
      "type": "opencard",
      "ricardian_contract": ""
    },{
      "name": "sayhi",
      "type": "sayhi",
      "ricardian_contract": ""
    },{
      "name": "printrand",
      "type": "printrand",
      "ricardian_contract": ""
    }
  ],
  "tables": [{
      "name": "global",
      "index_type": "i64",
      "key_names": [
        "id"
      ],
      "key_types": [
        "uint64"
      ],
      "type": "global"
    },{
      "name": "game",
      "index_type": "i64",
      "key_names": [
        "id"
      ],
      "key_types": [
        "uint64"
      ],
      "type": "game"
    }
  ],
  "ricardian_clauses": [],
  "error_messages": [],
  "abi_extensions": []
}