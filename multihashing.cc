#include <node.h>
#include <node_buffer.h>
#include <v8.h>

extern "C" {
    #include "bcrypt.h"
    #include "keccak.h"
    #include "quark.h"
    #include "scrypt.h"
    #include "scryptjane.h"
    #include "scryptn.h"
    #include "skein.h"
    #include "x11.h"
    #include "groestl.h"
    #include "blake.h"


    #define max(a,b)            (((a) > (b)) ? (a) : (b))
    #define min(a,b)            (((a) < (b)) ? (a) : (b))
    unsigned char GetNfactorJane(int nTimestamp, int nChainStartTime, int nMin, int nMax) {

            const unsigned char minNfactor = nMin;//4;
            const unsigned char maxNfactor = nMax;//30;

            int l = 0, s, n;
            unsigned char N;

            if (nTimestamp <= nChainStartTime)
                    return 4;

            s = nTimestamp - nChainStartTime;
            while ((s >> 1) > 3) {
                    l += 1;
                    s >>= 1;
            }

            s &= 3;

            n = (l * 170 + s * 25 - 2320) / 100;

            if (n < 0) n = 0;

            if (n > 255)
                    printf("GetNfactor(%d) - something wrong(n == %d)\n", nTimestamp, n);

            N = (unsigned char)n;
            //printf("GetNfactor: %d -> %d %d : %d / %d\n", nTimestamp - nChainStartTime, l, s, n, min(max(N, minNfactor), maxNfactor));

            return min(max(N, minNfactor), maxNfactor);
    }

    void scryptjane_hash(const void* input, size_t inputlen, uint32_t *res, unsigned char Nfactor)
    {
            return scrypt((const unsigned char*)input, inputlen,
                    (const unsigned char*)input, inputlen,
                    Nfactor, 0, 0, (unsigned char*)res, 32);
    }
}

using namespace node;
using namespace v8;

Handle<Value> except(const char* msg) {
    return ThrowException(Exception::Error(String::New(msg)));
}

Handle<Value> quark(const Arguments& args) {
    HandleScope scope;

    if (args.Length() < 1)
        return except("You must provide one argument.");

    Local<Object> target = args[0]->ToObject();

    if(!Buffer::HasInstance(target))
        return except("Argument should be a buffer object.");

    char * input = Buffer::Data(target);
    char * output = new char[32];
    
    unsigned int input_len = Buffer::Length(target);

    quark_hash(input, output, input_len);

    Buffer* buff = Buffer::New(output, 32);
    return scope.Close(buff->handle_);
}

Handle<Value> x11(const Arguments& args) {
    HandleScope scope;

    if (args.Length() < 1)
        return except("You must provide one argument.");

    Local<Object> target = args[0]->ToObject();

    if(!Buffer::HasInstance(target))
        return except("Argument should be a buffer object.");

    char * input = Buffer::Data(target);
    char * output = new char[32];

    unsigned int input_len = Buffer::Length(target);

    x11_hash(input, output, input_len);

    Buffer* buff = Buffer::New(output, 32);
    return scope.Close(buff->handle_);
}

Handle<Value> scrypt(const Arguments& args) {
   HandleScope scope;

   if (args.Length() < 1)
       return except("You must provide one argument.");

   Local<Object> target = args[0]->ToObject();

   if(!Buffer::HasInstance(target))
       return except("Argument should be a buffer object.");

   char * input = Buffer::Data(target);
   char * output = new char[32];

   scrypt_1024_1_1_256(input, output);

   Buffer* buff = Buffer::New(output, 32);
   return scope.Close(buff->handle_);
}



Handle<Value> scryptn(const Arguments& args) {
   HandleScope scope;

   if (args.Length() < 2)
       return except("You must provide buffer to hash and N factor.");

   Local<Object> target = args[0]->ToObject();

   if(!Buffer::HasInstance(target))
       return except("Argument should be a buffer object.");

   Local<Number> num = args[1]->ToNumber();
   unsigned int nFactor = num->Value();

   char * input = Buffer::Data(target);
   char * output = new char[32];

   //unsigned int N = 1 << (getNfactor(input) + 1);
   unsigned int N = 1 << nFactor;

   scrypt_N_1_1_256(input, output, N);


   Buffer* buff = Buffer::New(output, 32);
   return scope.Close(buff->handle_);
}

Handle<Value> scryptjane(const Arguments& args) {
    HandleScope scope;

    if (args.Length() < 5)
        return except("You must provide two argument: buffer, timestamp as number, and nChainStarTime as number, nMin, and nMax");

    Local<Object> target = args[0]->ToObject();

    if(!Buffer::HasInstance(target))
        return except("First should be a buffer object.");

    Local<Number> num = args[1]->ToNumber();
    int timestamp = num->Value();

    Local<Number> num2 = args[2]->ToNumber();
    int nChainStartTime = num2->Value();

    Local<Number> num3 = args[3]->ToNumber();
    int nMin = num3->Value();

    Local<Number> num4 = args[4]->ToNumber();
    int nMax = num4->Value();

    char * input = Buffer::Data(target);
    char * output = new char[32];

    scryptjane_hash(input, 80, (uint32_t *)output, GetNfactorJane(timestamp, nChainStartTime, nMin, nMax));

    Buffer* buff = Buffer::New(output, 32);
    return scope.Close(buff->handle_);
}

Handle<Value> keccak(const Arguments& args) {
    HandleScope scope;

    if (args.Length() < 1)
        return except("You must provide one argument.");

    Local<Object> target = args[0]->ToObject();

    if(!Buffer::HasInstance(target))
        return except("Argument should be a buffer object.");

    char * input = Buffer::Data(target);
    char * output = new char[32];

    unsigned int dSize = Buffer::Length(target);

    keccak_hash(input, output, dSize);

    Buffer* buff = Buffer::New(output, 32);
    return scope.Close(buff->handle_);
}


Handle<Value> bcrypt(const Arguments& args) {
    HandleScope scope;

    if (args.Length() < 1)
        return except("You must provide one argument.");

    Local<Object> target = args[0]->ToObject();

    if(!Buffer::HasInstance(target))
        return except("Argument should be a buffer object.");

    char * input = Buffer::Data(target);
    char * output = new char[32];

    bcrypt_hash(input, output);

    Buffer* buff = Buffer::New(output, 32);
    return scope.Close(buff->handle_);
}

Handle<Value> skein(const Arguments& args) {
    HandleScope scope;

    if (args.Length() < 1)
        return except("You must provide one argument.");

    Local<Object> target = args[0]->ToObject();

    if(!Buffer::HasInstance(target))
        return except("Argument should be a buffer object.");

    char * input = Buffer::Data(target);
    char * output = new char[32];

    unsigned int input_len = Buffer::Length(target);
    
    skein_hash(input, output, input_len);

    Buffer* buff = Buffer::New(output, 32);
    return scope.Close(buff->handle_);
}


Handle<Value> groestl(const Arguments& args) {
    HandleScope scope;

    if (args.Length() < 1)
        return except("You must provide one argument.");

    Local<Object> target = args[0]->ToObject();

    if(!Buffer::HasInstance(target))
        return except("Argument should be a buffer object.");

    char * input = Buffer::Data(target);
    char * output = new char[32];
    
    unsigned int input_len = Buffer::Length(target);

    groestl_hash(input, output, input_len);

    Buffer* buff = Buffer::New(output, 32);
    return scope.Close(buff->handle_);
}


Handle<Value> blake(const Arguments& args) {
    HandleScope scope;

    if (args.Length() < 1)
        return except("You must provide one argument.");

    Local<Object> target = args[0]->ToObject();

    if(!Buffer::HasInstance(target))
        return except("Argument should be a buffer object.");

    char * input = Buffer::Data(target);
    char * output = new char[32];
    
    unsigned int input_len = Buffer::Length(target);

    blake_hash(input, output, input_len);

    Buffer* buff = Buffer::New(output, 32);
    return scope.Close(buff->handle_);
}
void init(Handle<Object> exports) {
    exports->Set(String::NewSymbol("quark"), FunctionTemplate::New(quark)->GetFunction());
    exports->Set(String::NewSymbol("x11"), FunctionTemplate::New(x11)->GetFunction());
    exports->Set(String::NewSymbol("scrypt"), FunctionTemplate::New(scrypt)->GetFunction());
    exports->Set(String::NewSymbol("scryptn"), FunctionTemplate::New(scryptn)->GetFunction());
    exports->Set(String::NewSymbol("scryptjane"), FunctionTemplate::New(scryptjane)->GetFunction());
    exports->Set(String::NewSymbol("keccak"), FunctionTemplate::New(keccak)->GetFunction());
    exports->Set(String::NewSymbol("bcrypt"), FunctionTemplate::New(bcrypt)->GetFunction());
    exports->Set(String::NewSymbol("skein"), FunctionTemplate::New(skein)->GetFunction());
    exports->Set(String::NewSymbol("groestl"), FunctionTemplate::New(groestl)->GetFunction());
    exports->Set(String::NewSymbol("blake"), FunctionTemplate::New(blake)->GetFunction());
}

NODE_MODULE(multihashing, init)
