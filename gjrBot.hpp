#ifndef __BOT_GJR__
#define __BOT_GJR__

namespace gjrBot
{
	static const int dx[5]={0,-1,0,1,0};
	static const int dy[5]={0,0,-1,0,1};
	static const int dxx[8]={-1,-1,-1,0,0,1,1,1};
	static const int dyy[8]={-1,0,1,-1,1,-1,0,1};
	static const double Pi=3.14159265358979323846;
	static const int inf=1000000000;
	static int H,W;
	static int lastVisit[17][505][505];
	static int turn[17];
	static int lastDir[17];
	static int genx[17],geny[17];
	static int memgenx[17][17],memgeny[17][17];
	static bool canBack[17];
	static std::mt19937 mtrd(std::chrono::system_clock::now().time_since_epoch().count());
	int maxdis(int x,int y,int xx,int yy){return max(abs(x-xx),abs(y-yy));}
	int dis(int x,int y,int xx,int yy){return abs(x-xx)+abs(y-yy);}
	double angleDis(double x,double y){double dis=fabs(x-y);return min(dis,Pi+Pi-dis);}
	double revAngle(double x){if(x<Pi) return x+Pi;return x-Pi;}
	double getAngle(int x,int y,int xx,int yy){
		x=x-xx;y=yy-y;
		x^=y^=x^=y;
		if(x==0){
			if(y>0) return Pi*0.5;
			else return Pi*1.5;
		}
		if(x>0&&y>=0) return atan((double)y/(double)x);
		if(x>0) return Pi+Pi+atan((double)y/(double)x);
		if(y>=0) return Pi+atan((double)y/(double)x);
		return Pi+atan((double)y/(double)x);
	}
	bool ok(int x,int y){return x>0&&y>0&&x<=H&&y<=W;}
	bool canSee(int ind,int x,int y){
		for(int i=0;i<8;++i)
			if(gameMap[x+dxx[i]][y+dyy[i]].team==ind) return 1;
		return 0;
	}
	int calc(int ind,int t){
		switch(t){
			case 1:if(canBack[ind]) return 0;else return 500000000;
			case 2:return 100;
			case 3:return 60;
			case 4:return 30;
			case 5:return 20;
			case 6:return 10;
			case 7:return 5;
			case 8:return 2;
			default:return 0;
		}
	}
	int calc2(double tar,double dir){
		double dis=angleDis(tar,dir);
		if(dis<1.0) return sqrt(1.0-dis)*1000.0;
		return 0;
	}
	int gjrBot(int ind, playerCoord player){
		int x=player.x,y=player.y;
		int army=gameMap[x][y].army;
		++turn[ind];
		lastVisit[ind][x][y]=turn[ind];
		if(gameMap[x][y].team==ind&&gameMap[x][y].type==3) genx[ind]=x,geny[ind]=y;
		if(gameMap[x][y].team!=ind||army==0){
			memset(lastVisit[ind],0,sizeof(lastVisit[ind]));
			return lastDir[ind]=0;
		}
		for(int i=0;i<8;++i){
			int nx=x+dxx[i],ny=y+dyy[i];
			if(!ok(nx,ny)) continue;
			if(gameMap[nx][ny].type==3&&gameMap[nx][ny].team!=ind){
				int tar=gameMap[nx][ny].team;
				memgenx[ind][tar]=nx;
				memgeny[ind][tar]=ny;
			}
		}
		double Dir[17]={0},Genx[17]={0},Geny[17]={0};
		int Cnt[17]={0},Sum[17]={0};
		double Gx=0,Gy=0,Dirself;
		int land=0;
		for(int i=1;i<=H;++i)
			for(int j=1;j<=W;++j){
				int tm=gameMap[i][j].team;
				if(tm!=ind&&tm!=0&&canSee(ind,i,j)){
					++Cnt[tm];
					Genx[tm]+=i;Geny[tm]+=j;
					if(dis(i,j,genx[ind],geny[ind])<log2(turn[ind])/2+1) Sum[tm]+=gameMap[i][j].army-1;
				}
				if(tm==ind) ++land,Gx+=i,Gy+=j;
			}
		Gx/=(double)land;
		Gy/=(double)land;
		Dirself=getAngle(genx[ind],geny[ind],Gx,Gy);
		for(int i=1;i<=12;++i)
			if(Cnt[i]) Genx[i]/=(double)Cnt[i],Geny[i]/=(double)Cnt[i];
		for(int i=1;i<=12;++i)
			if(Cnt[i]) Dir[i]=getAngle(genx[ind],geny[ind],Genx[i],Geny[i]);
		for(int i=1;i<=12;++i)
			if(ok(memgenx[ind][i],memgeny[ind][i])) Dir[i]=getAngle(genx[ind],geny[ind],memgenx[ind][i],memgeny[ind][i]);
		for(int i=1;i<=12;++i)
			if(angleDis(Dir[i],Dirself)<1.5) Dir[i]=revAngle(Dir[i]);
		int target=0;
		for(int i=1;i<=12;++i)
			if(i!=ind&&Sum[i]>Sum[target]) target=i;
		H=mapH;W=mapW;
		int value[5];
		bool bk[5]={0};
		value[0]=-inf-inf;
		for(int d=1;d<=4;++d){
			int nx=x+dx[d],ny=y+dy[d];
			if(!ok(nx,ny)){
				value[d]=-inf;
				continue;
			}
			int tp=gameMap[nx][ny].type;
			int ar=gameMap[nx][ny].army;
			int tm=gameMap[nx][ny].team;
			if(tp==2){
				value[d]=-inf;
				continue;
			}
			if(tp==3){
				if(tm==ind){
					value[d]=-300000000;
					continue;
				}
				if(army-1>ar) value[d]=inf+ar;
				else value[d]=inf-ar;
				continue;
			}
			if(tp==4){
				if(tm==ind) value[d]=50+ar;
				else{
					if(army-2>ar){
						value[d]=50+turn[ind]/5+ar;
						bk[d]=1;
					}else value[d]=-100-(ar-army);
				}
				continue;
			}
			if(tp==1){
				value[d]=-inf+1;
				continue;
			}
			if(tp==0){
				if(tm==ind) value[d]=ar;
				else{
					if(tm==0) value[d]=10;
					else if(army-2>ar){
						value[d]=10+turn[ind]/25+ar;
						if(army-1-ar<=ar) bk[d]=1;
					}else value[d]=-100-turn[ind]/3-(ar-army);
				}
				continue;
			}
			value[d]=-inf-10000;
		}
		for(int d=1;d<=4;++d) value[d]-=calc(ind,turn[ind]-lastVisit[ind][x+dx[d]][y+dy[d]]);
		if(lastDir[ind]) value[(lastDir[ind]+1)%4+1]+=10+turn[ind]/25*2;
		for(int d=1;d<=4;++d){
			int nx=x+dx[d],ny=y+dy[d];
			if(ok(nx,ny)&&(gameMap[nx][ny].team!=ind&&(gameMap[nx][ny].type==0||(gameMap[nx][ny].team!=0&&gameMap[nx][ny].type==4)))&&maxdis(genx[ind],geny[ind],nx,ny)==1) value[d]+=turn[ind]+5;
		}
		if(target)
			for(int d=1;d<=4;++d){
				int nx=x+dx[d],ny=y+dy[d];
				if(ok(nx,ny)) value[d]+=calc2(Dir[target],getAngle(genx[ind],geny[ind],nx,ny));
			}
		for(int d=1;d<=4;++d) value[d]+=mtrd()%(int)(log2(turn[ind])+1);
		int p=0;
		for(int i=1;i<=4;++i)
			if(value[i]>value[p]) p=i;
		if(bk[p]) canBack[ind]=1;
		else canBack[ind]=0;
		return lastDir[ind]=p;
	}
}

#endif
