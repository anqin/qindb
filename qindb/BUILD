proto_library(
    name = 'schema_proto',
    srcs = 'schema.proto'
)

cc_library(
    name = 'qindb',
    srcs = [
        'arranger.cc',
        'counters.cc',
        'database.cc',
        'database_impl.cc',
        'filename.cc',
        'log_file.cc',
        'options.cc',
        'qin_cleaner.cc',
        'recover.cc',
        'status.cc',
    ],
    deps = [
        ':schema_proto',
        '//thirdparty/glog:glog',
        '//thirdparty/gflags:gflags',
        '//thirdparty/leveldb:leveldb',
        '//thirdparty/leveldb-1.9.0/util:util',
        '//toft/base/string:string',
        '//toft/storage/file:file',
        '//toft/container:counter',
        '//toft/system/threading:threading',
        '//toft/storage/recordio:recordio'
    ]
)
