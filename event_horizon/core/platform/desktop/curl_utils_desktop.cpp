//
// Created by Dado on 01/05/2018.
//

#include "../../http/curl_utils.h"
#include <cstring>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

namespace CurlUtil {

    enum class CurlRead {
        String,
        Binary
    };

    char *m_pBuffer = NULL;
    size_t m_Size = 0;

    void* Realloc(void* ptr, size_t size)
    {
        if(ptr)
            return realloc(ptr, size);
        else
            return malloc(size);
    };

    size_t WriteMemoryCallback(char* ptr, size_t size, size_t nmemb)
    {
        // Calculate the real size of the incoming buffer
        size_t realsize = size * nmemb;

        // (Re)Allocate memory for the buffer
        m_pBuffer = (char*) Realloc(m_pBuffer, m_Size + realsize);

        // Test if Buffer is initialized correctly & copy memory
        if (m_pBuffer == NULL) {
            realsize = 0;
        }

        std::memcpy(&(m_pBuffer[m_Size]), ptr, realsize);
        m_Size += realsize;

        // return the real size of the buffer...
        return realsize;
    };

    void readInternal( const std::string& url, [[maybe_unused]] CurlRead cr ) {

        try
        {
            // That's all that is needed to do cleanup of used resources (RAII style).
            curlpp::Cleanup myCleanup;

            // Our request to be sent.
            curlpp::Easy myRequest;

            free(m_pBuffer);
            m_pBuffer = NULL;
            m_Size = 0;

            // Set the URL.
            myRequest.setOpt<curlpp::OptionTrait<std::string, CURLOPT_URL>>(url);
            myRequest.setOpt<curlpp::options::UserAgent>("Chrome/60.0.3112.113");

            // Set the writer callback to enable cURL
            // to write result in a memory area
            curlpp::types::WriteFunctionFunctor functor(WriteMemoryCallback);
            curlpp::options::WriteFunction *test = new curlpp::options::WriteFunction(functor);
            myRequest.setOpt(test);

            // Send request and get a result.
            // By default the result goes to standard output.
            myRequest.perform();
        }

        catch(curlpp::RuntimeError & e)
        {
            std::cout << e.what() << std::endl;
        }

        catch(curlpp::LogicError & e)
        {
            std::cout << e.what() << std::endl;
        }
    }

    std::string read( const std::string& url ) {
        readInternal( url, CurlRead::String );
        return std::string(m_pBuffer);
    }

    const char* read( const std::string& url, uint64_t& size ) {
        readInternal( url, CurlRead::Binary );
        size = m_Size;
        return m_pBuffer;
    }
}