#include "src/QuoteBook.h"
#include "spdlog/spdlog.h"
#include "src/MyBookReader.h"

class MyEnv{
public:
    int a=1;
    int sharedValue=0;
    MyEnv()
    //: myquotebook_SERVER("JPY", true, "Server",{"AA", "B", "C", "A", "D", "kk"}, 2000,true),
    // myquotebook_CLIENT("JPY", false,"Client")
    {
        spdlog::info("Creating an Environment to test against");

       try{

           spdlog::info("Creating Env's.");
            myquotebook_SERVER = new QuoteBook("JPY", true, "Server", {"AA", "B", "C", "A", "D", "kk"}, 2000, true);
           spdlog::info("Created Server Env.");
            myquotebook_CLIENT = new QuoteBook("JPY", false, "Client");
           spdlog::info("Created Client Env.");
        }catch  (const std::exception& e)
        {
            spdlog::info("Exception. {}", e.what());
        }

    }

    void Replay(QuoteBook &book,std::vector<QuoteRecord> records)
    {

        spdlog::info("Environemnet Replay Function called.");
        spdlog::info("{} records to be replayed.",records.size());
        spdlog::info("QuoteBook Session name is {}.",book.Name);
        spdlog::info("QuoteBook Session type is {}.",book.SessionType);
        for (QuoteRecord record : records) {
            spdlog::info(" Side of record is {}", record.side);

            if (record.side == "BID") {
                book.BookAddBid(record.src, record.price,record.size);
            }else if (record.side == "OFFER") {
                book.BookAddOffer(record.src, record.price,record.size);
            } else {
                throw (std::invalid_argument("side"));

            }


        }




    }
    ~MyEnv(){}

    public:
        QuoteBook* myquotebook_SERVER;
        QuoteBook* myquotebook_CLIENT;



};
