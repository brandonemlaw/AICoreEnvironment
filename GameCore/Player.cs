using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GameCore
{
   

    public class Player
    {
        private identity playerIdentity;
        public bool turn = false;
     
        public void setPlayer(identity iden)
        {
            playerIdentity = iden;
        }

        public identity getIdentity()
        {
            return playerIdentity;
        }

        public virtual Move getMove()
        {
            Move result = new GameCore.Move();
            result.getFromUser(this);
            return result;
        }

        /*public char getIdentity()
        {
            if (playerIdentity == identity.X)
            {
                return Config.XCHAR;
            }
            else
            {
                return Config.OCHAR;
            }
            //return (char)playerIdentity;
        }*/
    }
}
