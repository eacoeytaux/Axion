#ifndef Porcupine_hpp
#define Porcupine_hpp

#include "Enemy.hpp"

namespace mtmercy
{

    class Porcupine : public Enemy
    {
    public:
        virtual ~Porcupine( ) { }
        Porcupine( World *, const Coordinate & position );

        virtual void render( ) override;

        virtual void update( ) override;

    private:
        varray<Line> m_quills;
    };

} // namespace mtmercy

#endif /* Porcupine_hpp */
