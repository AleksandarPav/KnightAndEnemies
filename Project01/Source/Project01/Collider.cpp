#include "Collider.h"
// header za sphere component;
#include "Components/SphereComponent.h"
// header za mesh component;
#include "Components/StaticMeshComponent.h"
// header za construcot helper, pri kreiranju static mesh-a;
#include "UObject/ConstructorHelpers.h"
// header za kontrolu kretanja pomocu user inputa;
#include "Components/InputComponent.h"
// header za spring arm;
#include "GameFramework/SpringArmComponent.h"
// header za camera;
#include "Camera/CameraComponent.h"
// header za movement component;
#include "ColliderMovementComponent.h"

// Sets default values
ACollider::ACollider()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// kreira scene component i inicijalizuje je;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	// kreiraj sphere component;
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	// attach sphere component to the root component;
	SphereComponent->SetupAttachment(GetRootComponent());
	// postavi radius sfere;
	SphereComponent->InitSphereRadius(40.f);
	// bira da mozse da se sudara sa certein things;
	SphereComponent->SetCollisionProfileName(TEXT("Pawn"));

	// kreiraj mesh component;
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	// attach mesh component to the root component;
	MeshComponent->SetupAttachment(GetRootComponent());

	// inace se static mesh postavlja u UE, i bolje je tako; ali ako hocemo sado-mazo preko cpp-a, ovako bi islo:
	// klasa je ConstructorHelpers, jedna od njenih metoda je FObjectFinder, ona je template;
	// kreira objekat MeshComponentAsset tipa UStaticMesh; prosledi joj se path do zeljenog static mesh-a;
	// pronalazenje path-a objasnjeno u wordu;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshComponentAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere'"));
	// inicijalizacija nase mesh component;
	// ako je succeeded, pronadjeno ono sto treba na prosledjenoj putanji;
	// MeshComponentAsset ima member variable called Object i tu je stored static mesh iz gornje path;
	if (MeshComponentAsset.Succeeded())
	{
		MeshComponent->SetStaticMesh(MeshComponentAsset.Object);
		// malo je veca i uzdignuta u odnosu na sferu za fiziku, pa cemo da je malo spustimo i smanjimo:
		MeshComponent->SetRelativeLocation(FVector(0.f, 0.f, -40.f));
		MeshComponent->SetWorldScale3D(FVector(0.8f, 0.8f, 0.8f));
	}

	// kreiraj spring arm;
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	// attach spring arm to the root component;
	SpringArm->SetupAttachment(GetRootComponent());
	// gledaj na pawna odozgo pod uglom od 45 stepeni;
	SpringArm->RelativeRotation = FRotator(-45.f, 0.f, 0.f);
	// drzi kameru na odredjenoj distanci;
	SpringArm->TargetArmLength = 400.f;
	// kamera blago kasni za igracem;
	SpringArm->bEnableCameraLag = true;
	// koliko da kamera kasni za igracem;
	SpringArm->CameraLagSpeed = 3.f;

	// kreiraj kameru;
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	// attach kameru to the spring arm;
	// drugi parametar je tipa FName; klasa SpringArmComponent ima member SocketName, tako da forma ispostovana;
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

	// kreiraj movement component;
	OurMovementComponent = CreateDefaultSubobject<UColliderMovementComponent>(TEXT("OurMovementComponent"));
	// set it to the root 'case that is the component we want to move when we add movement input to the movement component;
	OurMovementComponent->UpdatedComponent = RootComponent;

	// incijalizuj input sa rotacije misa;
	CameraInput = FVector2D(0.f, 0.f);

	//  kad krene Play, hocemo da se sve vidi iz perspektive kamere koja prati actora;
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void ACollider::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACollider::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// nova rotacija je trenutna rotacija, plus input koji stize rotacijom misa;
	FRotator NewRotation = GetActorRotation();
	NewRotation.Yaw += CameraInput.X;
	// ovo ce rotirati actora (sto i hocemo u slucaju misa levo-desno);
	SetActorRotation(NewRotation);

	// u slicaju gornje ose, ne zelimo da, kad idemo misem gore-dole, da se pomera i actor;
	// hocemo da se rotira samo spring arm koji drzi kameru;
	FRotator NewSpringArmRotation = SpringArm->GetComponentRotation();
	// izracunaj novu rotaciju;
	// ako ostane ovo, moci ce da se rotira i ispod actora sto nije dobro;
	/*NewSpringArmRotation.Pitch += CameraInput.Y;*/
	// clampuj novododeljenu vrednost da ne moze ispod -80 stepeni i iznad 15;
	NewSpringArmRotation.Pitch = FMath::Clamp(NewSpringArmRotation.Pitch += CameraInput.Y, -80.f, -15.f);
	// setuj novu rotaciju;
	SpringArm->SetWorldRotation(NewSpringArmRotation);

}

// Called to bind functionality to input
void ACollider::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// uvezi player input sa osama kretanja;
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ACollider::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ACollider::MoveRight);

	// bind kretanje sa pokretima misa;
	PlayerInputComponent->BindAxis(TEXT("CameraPitch"), this, &ACollider::PitchCamera);
	PlayerInputComponent->BindAxis(TEXT("CameraYaw"), this, &ACollider::YawCamera);
}

void ACollider::MoveForward(float Input)
{
	// get actor forward vector - vraca forward vector;
	FVector Forward = GetActorForwardVector();
	// f-ja koja dodaje movement; forward vektor se skalira inputom (izmedju -1 i 1);
	// PeekDefinition na f-ju, treba joj neki Movement Component;
	// zato kreirana klasa ColliderMovementComponent;
	// AddMovementInput(Input * Forward);
	// ovaj if paste-ovan iz f-je AddMovementInput, prilagodjen nasem slucaju (OurMovementComponent, Input i Forward);
	if (OurMovementComponent)
	{
		OurMovementComponent->AddInputVector(Input * Forward);
	}
}

void ACollider::MoveRight(float Input)
{
	// get actor right vector - vraca right vector;
	FVector Right = GetActorRightVector();
	// f-ja koja dodaje movement; right vektor se skalira inputom (izmedju -1 i 1);
	// PeekDefinition na f-ju, treba joj neki Movement Component;
	// zato kreirana klasa ColliderMovementComponent;
	// AddMovementInput(Input * Right);
	// ovaj if paste-ovan iz f-je AddMovementInput, prilagodjen nasem slucaju (OurMovementComponent, Input i Right);
	if (OurMovementComponent)
	{
		OurMovementComponent->AddInputVector(Input * Right);
	}
}

// rotaciono kretanje s pomeranjem misa;
void ACollider::PitchCamera(float AxisValue)
{
	CameraInput.Y = AxisValue;
}
void ACollider::YawCamera(float AxisValue)
{
	CameraInput.X = AxisValue;
}

// geter za movement component;
UPawnMovementComponent* ACollider::GetMovementComponent() const
{
	return OurMovementComponent;
}