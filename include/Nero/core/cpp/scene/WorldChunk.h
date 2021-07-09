////////////////////////////////////////////////////////////
// Nero Game Engine
// Copyright (c) 2016-2021 Sanou A. K. Landry
/////////////////////////////////////////////////////////////
#ifndef WORLDCHUNK_H
#define WORLDCHUNK_H
///////////////////////////HEADERS///////////////////////////
//Nero
//STD
#include <memory>
/////////////////////////////////////////////////////////////
namespace nero
{
	class WorldChunk
	{
		public: //utility
			typedef std::shared_ptr	<WorldChunk> Ptr;

		public:
								WorldChunk();
							   ~WorldChunk();

			//getters
			int					getChunkId()		const;
			std::string			getName()			const;
			bool				isVisible()			const;
			bool				isSelected()		const;
			bool				isLoadWithLevel()	const;
			//setters
			void				setChunkId(const int& chunkId);
			void				setName(const std::string& chunkName);
			void				setVisible(const bool& visible);
			void				setSelected(const bool& selected);
			void				setLoadWithLevel(const bool& loadWithLevel);

		private:
			friend class		GameLevelBuilder;
			int					m_ChunkId;
			std::string			m_ChunkName;
			bool				m_Visible;
			bool				m_Selected;
			bool				m_LoadWithLevel;
	};
}

#endif // WORLDCHUNK_H