#ifndef DEMO_HEADER_GUARD
#define DEMO_HEADER_GUARD

#include <vector>
#include <glm/glm.hpp>
#include <cstdio>

class GameDemo
{
public:
    GameDemo()
    {
        PC = 0;
		Cursor = glm::vec3(0,0,0);
    }
    
	struct Command
	{
		Command()
		{
			Cursor	= glm::vec3(0,0,0);
			ID		= -1;
			Tick	= 0;
		}

		Command(const Command& p_Copy)
		{
			Cursor	= p_Copy.Cursor;
			ID		= p_Copy.ID;
			Tick	= p_Copy.Tick;
		}

		Command(int& p_MessageID, glm::vec3& p_Cursor, int& p_Tick)
		{
			Cursor	= p_Cursor;
			ID		= p_MessageID;
			Tick	= p_Tick;
		}

		inline Command& operator=(const Command& p_Copy)
		{
			Cursor	= p_Copy.Cursor;
			ID		= p_Copy.ID;
			Tick	= p_Copy.Tick;

			return *this;
		}

		glm::vec3 	Cursor; //cursor location at this point
		int			ID;
		int			Tick;
	};

	void ReadFile(const char* p_FilePath)
	{
		Data.clear();

		FILE *demoFile = fopen(p_FilePath, "rb");
        Command swap;
        
		uint32_t length;
        fread(&length, sizeof(uint32_t), 1, demoFile);

        Data.resize(length, Command());
        
        fread(&Data[0], sizeof(Command) * length, 1,demoFile);
        
        fclose(demoFile);
        
        PC = 0;
	}

	void Save(const char* p_FilePath)
	{
		FILE *demoFile = fopen(p_FilePath, "wb");
        
        uint32_t size = Data.size();
        
        fwrite(&size, sizeof(uint32_t), 1, demoFile);
        fwrite(&Data[0], sizeof(Command)*size, 1, demoFile);
        
        fclose(demoFile);
	}

	inline void PushCommand(int p_MessageID, glm::vec3& p_Cursor,int& p_Tick)
	{
		Data.push_back(Command(p_MessageID, p_Cursor, p_Tick));
	}

    inline Command& operator[](size_t p_Index)
    {
        return Data[p_Index];
    }
    
    inline const Command& operator()(void)
    {
		Cursor = Data[PC].Cursor;
		PC++;
        return Data[PC-1];
    }
    
public:
	std::vector<Command>	Data;
    uint32_t                PC;     //program counter
	glm::vec3				Cursor;
};

#endif //#define DEMO_HEADER_GUARD
