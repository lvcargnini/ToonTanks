// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"


// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	// BaseMesh->SetupAttachment(CapsuleComp);
	RootComponent = ProjectileMesh;
	
	ProjectileMoveComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Component"));
	ProjectileMoveComp->MaxSpeed = 1300.f;
	ProjectileMoveComp->InitialSpeed = 1300.f;

	TrailParticles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particle System Component"));  
	TrailParticles->SetupAttachment(RootComponent);
}
	

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	ProjectileMesh->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);

	if(LaunchSound){
			UGameplayStatics::PlaySoundAtLocation(
				this,
				LaunchSound,
				GetActorLocation()
			);
		}


}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit){
	AActor* MyOwner = GetOwner();
	if(MyOwner == nullptr){
		Destroy();
		return;
	} 

	AController* MyOwnerInstigator = MyOwner->GetInstigatorController();
	UClass* DamageTypeClass = UDamageType::StaticClass();
	// UE_LOG(LogTemp, Display, TEXT("HitComp: %s"),*HitComp->GetName());
	// UE_LOG(LogTemp, Display, TEXT("OtherActor: %s"),*OtherActor->GetName());
	// UE_LOG(LogTemp, Display, TEXT("OtherComp: %s"),*OtherComp->GetName());

	if(OtherActor && (OtherActor != this) && (OtherActor!= MyOwner)){
		UGameplayStatics::ApplyDamage(
			OtherActor,
			Damage,
			MyOwnerInstigator,
			this,
			DamageTypeClass
		);
		if (HitParticle){
			UGameplayStatics::SpawnEmitterAtLocation(
				this,
				HitParticle,
				GetActorLocation(),
				GetActorRotation()
			);
		}
		if(HitSound){
			UGameplayStatics::PlaySoundAtLocation(
				this,
				HitSound,
				GetActorLocation()
			);
		}
		if(HitCameraShakeClass){
			GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(HitCameraShakeClass);
		}
		this->Destroy();
	}
}


